#ifndef _CPL_RNN_H
#define _CPL_RNN_H

#include "../cp-layers.h"


class RNN : public Layer {
private:
    int numGpuThreads;
    int numCpuThreads;
    floatN initfactor;
    int T,D,H,N;
    float maxClip=0.0;
    bool nohupdate;
    string hname;
    string hname0;
    void setup(const CpParams& cx) {
        cp=cx;
        layerName=cp.getPar("name",(string)"RNN");
        hname=layerName+"-h";
        hname0=layerName+"-h0";

        inputShapeRang=1;
        layerType=LayerType::LT_NORMAL | LayerType::LT_EXTERNALSTATE;
        vector<int> inputShape=cp.getPar("inputShape",vector<int>{});
        int inputShapeFlat=1;
        for (int j : inputShape) {
            inputShapeFlat *= j;
        }
        H=cp.getPar("H",1024);
        N=cp.getPar("N",1);
        XavierMode inittype=xavierInitType(cp.getPar("init",(string)"standard"));
        initfactor=cp.getPar("initfactor",(floatN)1.0);
        maxClip=cp.getPar("clip",(float)0.0);
        nohupdate=cp.getPar("nohupdate",(bool)false);  // true for auto-diff tests
        D=inputShape[0];
        T=inputShape[1];
        outputShape={H,T};

        cppl_set(&params, "Wxh", new MatrixN(xavierInit(MatrixN(D,H),inittype,initfactor)));
        cppl_set(&params, "Whh", new MatrixN(xavierInit(MatrixN(H,H),inittype,initfactor)));
        cppl_set(&params, "bh", new MatrixN(xavierInit(MatrixN(1,H),inittype,initfactor)));
        numGpuThreads=cpGetNumGpuThreads();
        numCpuThreads=cpGetNumCpuThreads();

        layerInit=true;
    }
public:
    RNN(const CpParams& cx) {
        setup(cx);
    }
    RNN(const string conf) {
        setup(CpParams(conf));
    }
    ~RNN() {
        cppl_delete(&params);
    }
    virtual void genZeroStates(t_cppl* pstates, int N) override {
        MatrixN *ph= new MatrixN(N,H);
        ph->setZero();
        cppl_set(pstates, hname, ph);
    }

    virtual MatrixN forward_step(const MatrixN& x, t_cppl* pcache, t_cppl* pstates, int id=0) {
        int N=shape(x)[0];
        MatrixN hprev = *(*pstates)[hname];
        MatrixN hnext = ((hprev * *params["Whh"] + x * *params["Wxh"]).rowwise() + RowVectorN(*params["bh"])).array().tanh();
        if (pcache != nullptr) {
            (*pcache)["x"] = new MatrixN(x);
            (*pcache)["hprev"] = new MatrixN(hprev);
            (*pcache)["hnext"] = new MatrixN(hnext);
        }
        return hnext;
    }

    MatrixN tensorchunk(const MatrixN& x, vector<int>dims, int b) {
        int A=dims[0];
        int C=dims[2];
        MatrixN xi(A,C);
        int a,c;
        for (a=0; a<A; a++) {
            for (c=0; c<C; c++) {
                xi(a,c) = x(a,b*C+c);
            }
        }
        return xi;
    }
    void tensorchunkinsert(MatrixN *ph, MatrixN& ht, vector<int>dims, int b) {
        int A=dims[0];
        int C=dims[2];
        int a,c;
        for (a=0; a<A; a++) {
            for (c=0; c<C; c++) {
                (*ph)(a,b*C+c) = ht(a,c);
            }
        }
    }

    virtual MatrixN forward(const MatrixN& x, t_cppl* pcache, t_cppl* pstates, int id=0) override {
        if (x.cols() != D*T) {
            cerr << layerName << ": " << "Forward: dimension mismatch in x:" << shape(x) << " D*T:" << D*T << ", D:" << D << ", T:" << T << ", H:" << H << endl;
            MatrixN h(0,0);
            return h;
        }
        int N=shape(x)[0];

        if (pstates->find(hname)==pstates->end()) {
            genZeroStates(pstates, N);  // If states[hname] is not defined, initialize to zero!
        }

        MatrixN ht=*(*pstates)[hname];

        MatrixN hn(N,T*H);
        for (int t=0; t<T; t++) {
            t_cppl cache{};
            t_cppl states{};
            states[hname]=&ht;
            MatrixN xi=tensorchunk(x,{N,T,D},t);
            ht = forward_step(xi,&cache,&states, id);
            tensorchunkinsert(&hn, ht, {N,T,H}, t);
            if (pcache!=nullptr) {
                string name="t"+std::to_string(t);
                mlPush(name,&cache,pcache);
            } else cppl_delete(&cache);
        }
        cppl_update(pstates,hname,&ht);
        if (hn.cols() != (T*H)) {
            cerr << "Inconsistent RNN-forward result: " << shape(hn) << endl;
        }
        return hn;
    }

    virtual MatrixN backward_step(const MatrixN& dchain, t_cppl* pcache, t_cppl* pstates, t_cppl* pgrads, int id=0) {
        if (pcache->find("x") == pcache->end()) {
            cerr << "cache does not contain x -> fatal!" << endl;
        }
        MatrixN x(*(*pcache)["x"]);
        MatrixN hprev(*(*pcache)["hprev"]);
        MatrixN hnext(*(*pcache)["hnext"]);
        MatrixN Wxh(*params["Wxh"]);
        MatrixN Whh(*params["Whh"]);
        MatrixN bh(*params["bh"]);

        MatrixN hsq = hnext.array() * hnext.array();
        MatrixN hone = MatrixN(hnext);
        hone.setOnes();

        MatrixN t1=(hone-hsq).array() * dchain.array();
        MatrixN t1t=t1.transpose();
        MatrixN dbh=t1.colwise().sum();
        MatrixN dx=(Wxh * t1t).transpose();
        MatrixN dWxh=(t1t * x).transpose();
        MatrixN dh=(Whh * t1t).transpose();
        MatrixN dWhh=hprev.transpose() * t1;

        (*pgrads)["Wxh"] = new MatrixN(dWxh);
        (*pgrads)["Whh"] = new MatrixN(dWhh);
        (*pgrads)["bh"] = new MatrixN(dbh);
        (*pgrads)[hname0] = new MatrixN(dh);

        if (maxClip != 0.0) {
            for (int i=0; i<dx.size(); i++) {
                if (dx(i) < -1.0 * maxClip) dx(i)=-1.0*maxClip;
                if (dx(i) > maxClip) dx(i)=maxClip;
            }
            for (int i=0; i<(*(*pgrads)["Wxh"]).size(); i++) {
                if ((*(*pgrads)["Wxh"])(i) < -1.0 * maxClip) (*(*pgrads)["Wxh"])(i)=-1.0*maxClip;
                if ((*(*pgrads)["Wxh"])(i) > maxClip) (*(*pgrads)["Wxh"])(i)=maxClip;
            }
            for (int i=0; i<(*(*pgrads)["Whh"]).size(); i++) {
                if ((*(*pgrads)["Whh"])(i) < -1.0 * maxClip) (*(*pgrads)["Whh"])(i)=-1.0*maxClip;
                if ((*(*pgrads)["Whh"])(i) > maxClip) (*(*pgrads)["Whh"])(i)=maxClip;
            }
            for (int i=0; i<(*(*pgrads)["bh"]).size(); i++) {
                if ((*(*pgrads)["bh"])(i) < -1.0 * maxClip) (*(*pgrads)["bh"])(i)=-1.0*maxClip;
                if ((*(*pgrads)["bh"])(i) > maxClip) (*(*pgrads)["bh"])(i)=maxClip;
            }
            for (int i=0; i<(*(*pgrads)[hname0]).size(); i++) {
                if ((*(*pgrads)[hname0])(i) < -1.0 * maxClip) (*(*pgrads)[hname0])(i)=-1.0*maxClip;
                if ((*(*pgrads)[hname0])(i) > maxClip) (*(*pgrads)[hname0])(i)=maxClip;
            }
        }

        return dx;
    }

    virtual MatrixN backward(const MatrixN& dchain, t_cppl* pcache, t_cppl* pstates, t_cppl* pgrads, int id=0) override {
        MatrixN dWxh,dWhh,dbh;
        string name;
        int N=shape(dchain)[0];
        MatrixN dhi,dxi,dphi;
        MatrixN dx(N,T*D);
        dx.setZero();
        for (int t=T-1; t>=0; t--) {
            t_cppl cache{};
            t_cppl grads{};
            t_cppl states{};
            if (t==T-1) {
                dhi=tensorchunk(dchain,{N,T,H},t);
            } else {
                dhi=tensorchunk(dchain,{N,T,H},t) + dphi;
            }
            name="t"+std::to_string(t);
            mlPop(name,pcache,&cache);
            dxi=backward_step(dhi,&cache,&states, &grads,id);
            tensorchunkinsert(&dx, dxi, {N,T,D}, t);
            dphi=*grads[hname0];
            if (t==T-1) {
                dWxh=*grads["Wxh"];
                dWhh=*grads["Whh"];
                dbh=*grads["bh"];
            } else {
                dWxh+=*grads["Wxh"];
                dWhh+=*grads["Whh"];
                dbh+=*grads["bh"];
            }
            cppl_delete(&grads);
        }
        (*pgrads)["Wxh"] = new MatrixN(dWxh);
        (*pgrads)["Whh"] = new MatrixN(dWhh);
        (*pgrads)["bh"] = new MatrixN(dbh);
        (*pgrads)[hname0] = new MatrixN(dphi);
        return dx;
    }
};

#endif
