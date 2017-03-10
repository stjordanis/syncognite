#include <iostream>
#include <fstream>
//#include <codecvt>
//#include <cstddef>
#include <locale>
#include <string>
#include <iomanip>
#include <ctime>

#include "cp-neural.h"

using std::wcout; using std::wstring;
using std::cerr; using std::vector;

class Text {
private:
    bool isinit=false;
    bool readDataFile(string inputfile) {
        std::wifstream txtfile(inputfile, std::ios::binary);
        if (!txtfile.is_open()) {
            return false;
        }
        txtfile.imbue(std::locale("en_US.UTF8"));
        wstring ttext((std::istreambuf_iterator<wchar_t>(txtfile)),
                     std::istreambuf_iterator<wchar_t>());
        txtfile.close();
        text=ttext;
        filename=inputfile;
        return true;
    }
public:
    wstring text;
    string filename;
    std::map<wchar_t,int> freq;
    std::map<wchar_t,int> w2v;
    std::map<int,wchar_t> v2w;
    Text(string filename) {
        if (readDataFile(filename)) {
            for (auto wc : text) {
                freq[wc]++;
            }
            int it=0;
            for (auto wc : freq) {
                w2v[wc.first]=it;
                v2w[it]=wc.first;
                ++it;
            }
            isinit=true;
            cerr << "Freq:" << freq.size() << ", w2v:" << w2v.size() << ", v2w:" << v2w.size() << endl;
        }
    }
    ~Text() {

    }
    bool isInit() {
        return isinit;
    }
    int vocsize() {
        if (!isinit) return 0;
        return v2w.size();
    }

    wstring sample(int len) {
        int p=std::rand()%(text.size()-len);
        return text.substr(p,len);
    }
};

int main(int argc, char *argv[]) {
    std::setlocale (LC_ALL, "");
    wcout << L"Rnn-Readär" << std::endl;

    bool allOk=true;
    if (argc!=2) {
        cerr << "rnnreader <path-to-text-file>" << endl;
        exit(-1);
    }
    Text txt(argv[1]);
    if (!txt.isInit()) {
        cerr << "Cannot initialize Text object from inputfile: " << argv[1] << endl;
        exit(-1);
    }

    wcout << L"Text size: " << txt.text.size() << endl;

    wcout << L"Size of vocabulary:" << txt.freq.size() << std::endl;
/*    for (auto f : txt.freq) {
        int c=(int)f.first;
        wstring wc(1,f.first);
        wcout << wc << L"|" <<  wchar_t(f.first) << L"(0x" << std::hex << c << L")" ": " << std::dec <<  f.second << endl;
    }
*/
    Color::Modifier red(Color::FG_RED);
    Color::Modifier green(Color::FG_GREEN);
    Color::Modifier def(Color::FG_DEFAULT);

    int T=30;
    int N=txt.text.size() / (T+1);
    cerr << N << " Max datassets" << endl;
    MatrixN Xr(N,T);
    MatrixN yr(N,T);

    wstring chunk,chunky;
    int n=0;
    for (int i=0; i<N; i++) {
        wstring smp = txt.sample(T+1);
        chunk=smp.substr(0,T);
        chunky=smp.substr(1,T);
        for (int t=0; t<T; t++) {
            Xr(i,t)=txt.w2v[chunk[t]];
            yr(i,t)=txt.w2v[chunky[t]];
        }
        ++n;
    }

    int maxN = 100000;
    if (n>maxN) n=maxN;
    int n1=n*0.9;
    int dn=(n-n1)/2;
    if (n1+2*dn > n) {
        cerr << "Math doesn't work out." << endl;
    }

    cerr << n1 << " datasets" << endl;

/*    MatrixN X(n1,T);
    MatrixN y(n1,T);
    MatrixN Xv(dn,T);
    MatrixN yv(dn,T);
    MatrixN Xt(dn,T);
    MatrixN yt(dn,T);
*/
    MatrixN X=Xr.block(0,0,n1,T);
    MatrixN y=yr.block(0,0,n1,T);
    MatrixN Xv=Xr.block(n1,0,dn,T);
    MatrixN yv=yr.block(n1,0,dn,T);
    MatrixN Xt=Xr.block(n1+dn,0,dn,T);
    MatrixN yt=yr.block(n1+dn,0,dn,T);

    std::srand(std::time(0));

    cpInitCompute("Rnnreader");
    registerLayers();

    LayerBlock lb(R"({"name":"rnnreader","init":"normal","initfactor":0.01})"_json);
    int VS=txt.vocsize();
    int H=128;
    int BS=64;
    float clip=2.0;

    //int D=64;
    // CpParams cp0;
    // cp0.setPar("inputShape",vector<int>{T});
    // cp0.setPar("V",VS);
    // cp0.setPar("D",D);
    // cp0.setPar("clip",clip);
    // cp0.setPar("init",(string)"orthonormal");
    // lb.addLayer("WordEmbedding","WE0",cp0,{"input"});

    string rnntype="LSTM"; // or "RNN"
    cerr << "RNN-type: " << rnntype << endl;

    json j0;
    string oName{"OH0"};
    j0["inputShape"]=vector<int>{T};
    j0["V"]=VS;
    lb.addLayer("OneHot",oName,j0,{"input"});

    int layer_depth=2;
    string nName;
    json j1;
    j1["inputShape"]=vector<int>{VS,T};
    j1["N"]=BS;
    j1["H"]=H;
    j1["forgetgateinitones"]=false;
    j1["clip"]=clip;
    for (auto l=0; l<layer_depth; l++) {
        nName="lstm"+std::to_string(l);
        lb.addLayer(rnntype,nName,j1,{oName});
        oName=nName;
    }

    json j10;
    j10["inputShape"]=vector<int>{H,T};
    j10["M"]=VS;
    lb.addLayer("TemporalAffine","af1",j10,{oName});

    json j11;
    j11["inputShape"]=vector<int>{VS,T};
    lb.addLayer("TemporalSoftmax","sm1",j11,{"af1"});

    if (!lb.checkTopology(true)) {
        allOk=false;
        cerr << red << "Topology-check for LayerBlock: ERROR." << def << endl;
    } else {
        cerr << green << "Topology-check for LayerBlock: ok." << def << endl;
    }

    //json jc;
    //lb.getLayerConfiguration(jc);
    //cerr << jc.dump(4) << endl;

    // preseverstates no longer necessary for training!
    json jo(R"({"verbose":true,"shuffle":false,"preservestates":false,"epsilon":1e-8})"_json);
    jo["learning_rate"]=(floatN)1e-3; //2.2e-2);

    floatN dep=2.0;
    floatN sep=0.0;
    jo["epochs"]=(floatN)dep;
    jo["batch_size"]=BS;

    for (int i=0; i<10000; i++) {
        jo["startepoch"]=(floatN)sep;
        t_cppl states;
        t_cppl statesv;
        states["y"] = new MatrixN(y);
        statesv["y"] = new MatrixN(yv);
        floatN cAcc=lb.train(X, &states, Xv, &statesv, "Adam", jo);
        cppl_delete(&states);
        cppl_delete(&statesv);

        sep+=dep;

        int pos=rand() % 1000 + 5000;
        wstring instr=txt.text.substr(pos,T);

        MatrixN xg(1,T);
        for (int i=0; i<T; i++) {
            xg(0,i)=txt.w2v[instr[i]];
        }
        wstring sout{};
        Layer* plstm0=lb.layerMap["lstm0"];
        t_cppl statesg{};
        plstm0->genZeroStates(&statesg, 1);

        for (int g=0; g<1000; g++) {
            t_cppl cache{};

            MatrixN probst=lb.forward(xg,&cache, &statesg);
            MatrixN probsd=MatrixN(T,VS);
            for (int t=0; t<T; t++) {
                for (int v=0; v<VS; v++) {
                    probsd(t,v)=probst(0,t*VS+v);
                }
            }
            int li=-1;
            for (int t=0; t<T; t++) {
                vector<floatN> probs(VS);
                vector<floatN> index(VS);
                for (int v=0; v<VS; v++) {
                    probs[v]=probsd(t,v);
                    index[v]=v;
                }
                li=(int)index[randomChoice(index, probs)];

                //wchar_t cw=txt.v2w[li];
                //wcout << cw;
            }
            //wcout <<  endl;
            cppl_delete(&cache);

            for (int t=0; t<T-1; t++) {
                xg(0,t)=xg(0,t+1);
            }
            xg(0,T-1)=li;
            sout += txt.v2w[li];
        }
        wcout << "output: " << sout << endl;
        cppl_delete(&statesg);
    }
    cpExitCompute();
}
