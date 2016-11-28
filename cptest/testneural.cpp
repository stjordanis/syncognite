#include <cp-neural.h>

#include "testneural.h"

// Manual build:
// g++ -g -ggdb -I ../cpneural -I /usr/local/include/eigen3 testneural.cpp -L ../Build/cpneural/ -lcpneural -lpthread -o test

bool matComp(MatrixN& m0, MatrixN& m1, string msg, floatN eps) {
    if (m0.cols() != m1.cols() || m0.rows() != m1.rows()) {
        cerr << msg << ": Incompatible shapes " << shape(m0) << "!=" << shape(m1) << endl;
        return false;
    }
    MatrixN d = m0 - m1;
    floatN dif = d.cwiseProduct(d).sum();
    if (dif < eps) {
        cerr << msg << " err=" << dif << endl;
        return true;
    } else {
        IOFormat CleanFmt(4, 0, ", ", "\n", "[", "]");
        cerr << msg << " m0:" << endl << m0.format(CleanFmt) << endl;
        cerr << msg << " m1:" << endl << m1.format(CleanFmt) << endl;
        cerr << msg << "  ∂:" << endl << (m0-m1).format(CleanFmt) << endl;
        cerr << "err=" << dif << endl;
        return false;
    }
}

bool registerTest() {
    bool allOk=true;
    cerr << "Registered Layers:" << endl;
    int nr=1;
    for (auto it : _syncogniteLayerFactory.mapl) {
        cerr << nr << ".: " << it.first << " ";
        t_layer_props_entry te=_syncogniteLayerFactory.mapprops[it.first];
        CpParams cp;
        cp.setPar("inputShape",std::vector<int>(te));
        Layer *l = CREATE_LAYER(it.first, cp)
        if (l->layerType == LT_NORMAL) {
            cerr << "normal layer" << endl;
        } else if (l->layerType==LT_LOSS) {
            cerr << "loss-layer (final)" << endl;
        } else {
            cerr << "unspecified layer -- ERROR!" << endl;
            allOk=false;
        }
        delete l;
        ++nr;
    }
    return allOk;
}

int tFunc(floatN x, int c) {
    int y=(int)(((sin(x/5.0)+1.0)/2.0)*(floatN)c);
    //cerr << x << ":" << y << " " << endl;
    return y;
}

bool trainTest() {
    bool allOk=true;
    CpParams cp;
    int N=300,NV=30,NT=30,I=5,H=20,C=4;
    cp.setPar("inputShape",vector<int>{I});
    cp.setPar("hidden",vector<int>{H,C});
    TwoLayerNet tln(cp);

    MatrixN X(N,I);
    X.setRandom();
    MatrixN y(N,1);
    for (unsigned i=0; i<y.rows(); i++) y(i,0)=tFunc(X(i,0),C);

    MatrixN Xv(NV,I);
    Xv.setRandom();
    MatrixN yv(NV,1);
    for (unsigned i=0; i<yv.rows(); i++) yv(i,0)=tFunc(Xv(i,0),C);

    MatrixN Xt(NT,I);
    Xt.setRandom();
    MatrixN yt(NT,1);
    for (unsigned i=0; i<yt.rows(); i++) yt(i,0)=tFunc(Xt(i,0),C);

    CpParams cpo("{verbose=false;epochs=200.0;batch_size=20;learning_rate=1e-2;"\
                "lr_decay=1.0;momentum=0.9;decay_rate=0.98;epsilon=1e-8;threads=2}");

    floatN train_err,test_err,val_err;

    tln.train(X, y, Xv, yv, "Adam", cpo);
    //tln.train(X, y, Xv, yv, "Sdg", cpo);
    train_err=tln.test(X, y);
    val_err=tln.test(Xv, yv);
    test_err=tln.test(Xt, yt);

    cerr << "Train-test, train-err=" << train_err << endl;
    cerr << "       validation-err=" << val_err << endl;
    cerr << "       final test-err=" << val_err << endl;
    if (test_err>0.2 || val_err>0.2 || train_err>0.2) allOk=false;
    return allOk;
}

int doTests() {
    MatrixN yz=MatrixN(0,0);
    cerr << "=== 0.: Init: registering layers" << endl;
    registerLayers();
    cerr << "=== 1.: Numerical gradient tests" << endl;
    bool allOk=true;
    Color::Modifier red(Color::FG_RED);
    Color::Modifier green(Color::FG_GREEN);
    Color::Modifier def(Color::FG_DEFAULT);

    Affine pc(CpParams("{inputShape=[30];hidden=20}"));
    MatrixN x(10,30);
    x.setRandom();
    if (!pc.selfTest(x,yz)) {
        allOk=false;
    }

    Relu rl(CpParams("{inputShape=[20]}"));
    MatrixN xr(10,20);
    xr.setRandom();
    if (!rl.selfTest(xr,yz)) {
        allOk=false;
    }

    vector<string> nls={"relu","sigmoid","tanh"};
    for (string nlsi : nls) {
        CpParams cp("{inputShape=[20]}");
        cp.setPar("type", nlsi);
        Nonlinearity nlr(cp);
        MatrixN xnl(10,20);
        xnl.setRandom();
        if (!nlr.selfTest(xnl,yz)) {
            allOk=false;
        }
    }

    // Batchnorm - still some strangities:
    BatchNorm bn("{inputShape=[10];train=true;noVectorizationTests=true}");
    MatrixN xbr(20,10);
    xbr.setRandom();
    if (!bn.selfTest(xbr,yz, 1e-4, 1e-3)) {
        allOk=false;
    }

    // Dropout
    Dropout dp("{inputShape=[5];train=true;noVectorizationTests=true;freeze=true;drop=0.8}");
    MatrixN xdp(3,5);
    xdp.setRandom();
    floatN h=1e-6; if (h<CP_DEFAULT_NUM_H) h=CP_DEFAULT_NUM_H;
    floatN eps=1e-8; if (eps<CP_DEFAULT_NUM_EPS) eps=CP_DEFAULT_NUM_EPS;
    if (!dp.selfTest(xdp,yz, h, eps)) {
        allOk=false;
    }

    // Convolution
    //Convolution cv("{inputShape=[3,4,4,16,3,3];stride=1;pad=0}");
    //MatrixN xcv(20,48);
    Convolution cv("{inputShape=[3,5,5];kernel=[2,3,3];stride=1;pad=1}");
    MatrixN xcv(2,75);
    xcv.setRandom();
    if (!cv.selfTest(xcv, yz, 1e-2, 1e-3)) {
        allOk=false;
    }

    // Pooling
    Pooling pl("{inputShape=[3,4,4];stride=2}");
    MatrixN xpl(20,48);
    xpl.setRandom();
    if (!pl.selfTest(xpl, yz)) {
        allOk=false;
    }

    // SpatialBatchNorm
    SpatialBatchNorm sbn("{inputShape=[3,4,4];train=true;N=2;noVectorizationTests=true}");
    MatrixN xsbn(2,3*4*4);
    xsbn.setRandom();
    if (!sbn.selfTest(xsbn, yz)) {
        allOk=false;
    }

    // RNN
    int rnnN=4;
    MatrixN xrnn(rnnN,5*7);
    MatrixN h0(rnnN,6);
    xrnn.setRandom();
    h0.setRandom();
    //                    D,T
    RNN rnn("{inputShape=[5,7];H=6;N=4;noVectorizationTests=true}");
    *(rnn.params["ho"])=h0;
    if (!rnn.selfTest(xrnn, yz, 1e-2, 1e-3)) {
        allOk=false;
    }

    // WordEmbedding
    int weN=4, weT=3, weV=10, weD=8;
    MatrixN xwe(weN,weT);
    MatrixN weW(weV,weD);
    xwe.setRandom();
    weW.setRandom();
    WordEmbedding we("{inputShape=[3];V=10;D=8;noVectorizationTests=true}");
    *(we.params["W"])=weW;
    if (!we.selfTest(xwe, yz, 1e-2, 1e-3)) {
        allOk=false;
    }

    // N=10; T=5; D=6; M=7
    // TemporalAffine pct(CpParams("{inputShape=[30];T=5;D=6;M=7;noVectorizationTests=true}")); // 30=T*D
    TemporalAffine pct(CpParams("{inputShape=[6,5];M=7}")); // T=5;D=6;30=T*D
    MatrixN xtt(10,30);
    xtt.setRandom();
    if (!pct.selfTest(xtt,yz)) {
        allOk=false;
    }

    AffineRelu rx("{inputShape=[2];hidden=3}");
    MatrixN xarl(30,2);
    xarl.setRandom();
    h=1e-6; if (h<CP_DEFAULT_NUM_H) h=CP_DEFAULT_NUM_H;
    eps=1e-6; if (eps<CP_DEFAULT_NUM_EPS) eps=CP_DEFAULT_NUM_EPS;
    if (!rx.selfTest(xarl,yz, h, eps)) {
        allOk=false;
    }

    // TwoLayerNet
    int ntl1=4, ntl2=5, ntl3=6, ntlN=30;
    CpParams tcp;
    tcp.setPar("inputShape",vector<int>{ntl1});
    tcp.setPar("hidden",vector<int>{ntl2,ntl3});
    TwoLayerNet tl(tcp);
    MatrixN xtl(ntlN,ntl1);
    xtl.setRandom();
    MatrixN y2(ntlN,1);
    for (unsigned i=0; i<y2.rows(); i++) y2(i,0)=(rand()%ntl3);
    h=1e-3; if (h<CP_DEFAULT_NUM_H) h=CP_DEFAULT_NUM_H;
    eps=1e-5; if (eps<CP_DEFAULT_NUM_EPS) eps=CP_DEFAULT_NUM_EPS;
    if (!tl.selfTest(xtl,y2, h, eps)) {
        allOk=false;
        cerr << red << "Numerical gradient for TwoLayerNet: ERROR." << def << endl;
    }

    // Softmax
    int smN=10, smC=4;
    CpParams c1;
    c1.setPar("inputShape",vector<int>{smC});
    Softmax mx(c1);
    MatrixN xmx(smN,smC);
    xmx.setRandom();
    MatrixN y(smN,1);
    for (unsigned i=0; i<y.rows(); i++) y(i,0)=(rand()%smC);
    h=1e-3; if (h<CP_DEFAULT_NUM_H) h=CP_DEFAULT_NUM_H;
    eps=1e-6; if (eps<CP_DEFAULT_NUM_EPS) eps=CP_DEFAULT_NUM_EPS;
    if (!mx.selfTest(xmx, y, h, eps)) {
        allOk=false;
    }

    // Temporal Softmax
    int tsmN=10, tsmC=4, Ttm=4;
    CpParams tc1;
    tc1.setPar("inputShape",vector<int>{smC,Ttm});
    tc1.setPar("noVectorizationTests", (bool)true);
    TemporalSoftmax tmx(tc1);
    MatrixN txmx(tsmN*Ttm,tsmC);
    txmx.setRandom();
    MatrixN ty(tsmN,Ttm);
    for (unsigned i=0; i<ty.size(); i++) ty(i,0)=(rand()%tsmC);
    h=1e-2; if (h<CP_DEFAULT_NUM_H) h=CP_DEFAULT_NUM_H;
    eps=1e-4; if (eps<CP_DEFAULT_NUM_EPS) eps=CP_DEFAULT_NUM_EPS;
    if (!tmx.selfTest(txmx, ty, h, eps)) {
        allOk=false;
    }

    // SVM
    int svN=10, svC=5;
    CpParams c2;
    c2.setPar("inputShape",vector<int>{svC});
    Svm sv(c2);
    MatrixN xsv(svN,svC);
    xsv.setRandom();
    MatrixN yv(svN,1);
    for (unsigned i=0; i<yv.rows(); i++) yv(i,0)=(rand()%svC);
    h=1e-3; if (h<CP_DEFAULT_NUM_H) h=CP_DEFAULT_NUM_H;
    eps=1e-6; if (eps<CP_DEFAULT_NUM_EPS) eps=CP_DEFAULT_NUM_EPS;
    if (!sv.selfTest(xsv, yv, h, eps)) {
        allOk=false;
    }

    //LayerBlock1
    LayerBlock lb("{name='testblock'}");
    cerr << "LayerName for lb: " << lb.layerName << endl;
    lb.addLayer("Affine","af1","{inputShape=[10]}",{"input"});
    lb.addLayer("Relu","rl1","",{"af1"});
    lb.addLayer("Affine","af2","{hidden=10}",{"rl1"});
    lb.addLayer("Softmax","sm1","",{"af2"});
    if (!lb.checkTopology(true)) {
        allOk=false;
        cerr << red << "Topology-check for LayerBlock: ERROR." << def << endl;
    } else {
        cerr << green << "Topology-check for LayerBlock: ok." << def << endl;
    }
    MatrixN xml(5,10);
    xml.setRandom();
    MatrixN yml(5,1);
    for (unsigned i=0; i<yml.rows(); i++) yml(i,0)=(rand()%10);

    h=1e-3; if (h<CP_DEFAULT_NUM_H) h=CP_DEFAULT_NUM_H;
    eps=1e-5; if (eps<CP_DEFAULT_NUM_EPS) eps=CP_DEFAULT_NUM_EPS;
    if (!lb.selfTest(xml,yml, h, eps)) {
        allOk=false;
        cerr << red << "Numerical gradient for LayerBlock: ERROR." << def << endl;
    }

    cerr << "=== 2.: Test-data tests" << endl;

    if (checkAffineForward()) {
        cerr << green << "AffineForward (Affine) with test data: OK." << def << endl;
    } else {
        cerr << red << "AffineForward (Affine) with test data: ERROR." << def << endl;
        allOk=false;
    }

    if (checkAffineBackward()) {
        cerr << green << "AffineBackward (Affine) with test data: OK." << def << endl;
    } else {
        cerr << red << "AffineBackward (Affine) with test data: ERROR." << def << endl;
        allOk=false;
    }

    if (checkReluForward()) {
        cerr << green << "ReluForward with test data: OK." << def << endl;
    } else {
        cerr << red << "ReluForward with test data: ERROR." << def << endl;
        allOk=false;
    }

    if (checkReluBackward()) {
        cerr << green << "ReluBackward (Affine) with test data: OK." << def << endl;
    } else {
        cerr << red << "ReluBackward (Affine) with test data: ERROR." << def << endl;
        allOk=false;
    }

    if (checkNonlinearityForward()) {
        cerr << green << "NonlinearityForward with test data: OK." << def << endl;
    } else {
        cerr << red << "NonlinearityForward with test data: ERROR." << def << endl;
        allOk=false;
    }

    if (checkNonlinearityBackward()) {
        cerr << green << "NonlinearityBackward with test data: OK." << def << endl;
    } else {
        cerr << red << "NonlinearityBackward with test data: ERROR." << def << endl;
        allOk=false;
    }

    if (checkBatchNormForward()) {
        cerr << green << "BatchNormForward with test data: OK." << def << endl;
    } else {
        cerr << red << "BatchNormForward with test data: ERROR." << def << endl;
        allOk=false;
    }

    if (checkBatchNormBackward()) {
        cerr << green << "BatchNormBackward with test data: OK." << def << endl;
    } else {
        cerr << red << "BatchNormBackward with test data: ERROR." << def << endl;
        allOk=false;
    }

    if (checkDropout()) {
        cerr << green << "Dropout with test data: OK." << def << endl;
    } else {
        cerr << red << "Dropout with test data: ERROR." << def << endl;
        allOk=false;
    }

    if (checkConvolutionForward()) {
        cerr << green << "ConvolutionForward (Convolution) with test data: OK." << def << endl;
    } else {
        cerr << red << "ConvolutionForward (Convolution) with test data: ERROR." << def << endl;
        allOk=false;
        exit(-1);
    }
    if (checkConvolutionBackward()) {
        cerr << green << "ConvolutionBackward (Convolution) with test data: OK." << def << endl;
    } else {
        cerr << red << "ConvolutionBackward (Convolution) with test data: ERROR." << def << endl;
        allOk=false;
    }

    if (checkPoolingForward()) {
        cerr << green << "PoolingForward with test data: OK." << def << endl;
    } else {
        cerr << red << "PoolingForward with test data: ERROR." << def << endl;
        allOk=false;
        exit(-1);
    }
    if (checkPoolingBackward()) {
        cerr << green << "PoolingBackward with test data: OK." << def << endl;
    } else {
        cerr << red << "PoolingBackward with test data: ERROR." << def << endl;
        allOk=false;
    }

    if (checkAffineRelu()) {
        cerr << green << "AffineRelu with test data: OK." << def << endl;
    } else {
        cerr << red << "AffineRelu with test data: ERROR." << def << endl;
        allOk=false;
    }

    if (checkSoftmax()) {
        cerr << green << "Softmax with test data: OK." << def << endl;
    } else {
        cerr << red << "Softmax with test data: ERROR." << def << endl;
        allOk=false;
    }
    if (checkSvm()) {
        cerr << green << "Svm with test data: OK." << def << endl;
    } else {
        cerr << red << "Svm with test data: ERROR." << def << endl;
        allOk=false;
    }

    if (checkTwoLayer()) {
        cerr << green << "TwoLayerNet with test data: OK." << def << endl;
    } else {
        cerr << red << "TwoLayerNet with test data: ERROR." << def << endl;
        allOk=false;
    }

    if (checkRNNStepForward()) {
        cerr << green << "RNNForwardStep with test data: OK." << def << endl;
    } else {
        cerr << red << "RNNForwardStep with test data: ERROR." << def << endl;
        allOk=false;
    }
    if (checkRNNStepBackward()) {
        cerr << green << "RNNBackwardStep with test data: OK." << def << endl;
    } else {
        cerr << red << "RNNBackwardStep with test data: ERROR." << def << endl;
        allOk=false;
    }
    if (checkRNNForward()) {
        cerr << green << "RNNForward with test data: OK." << def << endl;
    } else {
        cerr << red << "RNNForward with test data: ERROR." << def << endl;
        allOk=false;
    }
    if (checkRNNBackward()) {
        cerr << green << "RNNBackward with test data: OK." << def << endl;
    } else {
        cerr << red << "RNNBackward with test data: ERROR." << def << endl;
        allOk=false;
    }

    if (checkWordEmbeddingForward()) {
        cerr << green << "WordEmbeddingForward with test data: OK." << def << endl;
    } else {
        cerr << red << "WordEmbeddingForward with test data: ERROR." << def << endl;
        allOk=false;
    }
    if (checkWordEmbeddingBackward()) {
        cerr << green << "WordEmbeddingBackward with test data: OK." << def << endl;
    } else {
        cerr << red << "WordEmbeddingBackward with test data: ERROR." << def << endl;
        allOk=false;
    }

    if (checkTemporalAffineForward()) {
        cerr << green << "TemporalAffineForward with test data: OK." << def << endl;
    } else {
        cerr << red << "TemporalAffineForward with test data: ERROR." << def << endl;
        allOk=false;
    }

    if (checkTemporalAffineBackward()) {
        cerr << green << "TemporalAffineBackward with test data: OK." << def << endl;
    } else {
        cerr << red << "TemporalAffineBackward with test data: ERROR." << def << endl;
        allOk=false;
    }

    if (checkTemporalSoftmaxLoss(0.1)) {
        cerr << green << "TemporalSoftmaxLoss with test data: OK." << def << endl;
    } else {
        cerr << red << "TemporalSoftmaxLoss with test data: ERROR." << def << endl;
        allOk=false;
    }

    if (trainTest()) {
        cerr << green << "TrainTest: OK." << def << endl;
    } else {
        cerr << red << "TrainTest: ERROR." << def << endl;
        allOk=false;
    }


    if (registerTest()) {
        cerr << green << "RegisterTest: OK." << def << endl;
    } else {
        cerr << red << "RegisterTest: ERROR." << def << endl;
        allOk=false;
    }

    if (allOk) {
        cerr << green << "All tests ok." << def << endl;
    } else {
        cerr << red << "Tests failed." << def << endl;
    }

    return 0;
}

int main(int argc, char *argv[]) {
    string name="test";
    cpInitCompute(name);
    int ret=0;
    ret=doTests();
    cpExitCompute();
    return ret;
}
