#ifndef _CPLT_TEMPORALAFFINE_H
#define _CPLT_TEMPORALAFFINE_H

#include "../testneural.h"

bool checkTemporalAffineForward(floatN eps=CP_DEFAULT_NUM_EPS, int verbose=1) {
    // N, T, D, M = 2, 3, 4, 5
    MatrixN x(2,3*4);  // (N, T*D)
    x << -0.7461612 , -1.55144546,  1.82876302,  0.5294384,
          0.64588784,  1.28880436, -1.13547596, -1.49195431,
         -0.12854723,  0.02965153,  0.27540986, -0.16085281,

          0.9820497 ,  0.17045352, -1.58826521, -0.75047715,
         -0.44713369,  1.18263892,  0.51718481, -1.91610422,
         -0.6941147 , -1.27365114, -0.44673534, -1.04071134;
    MatrixN W(4,5); // (D,M)
    W << -0.11804466,  0.60008559,  0.64535532, -1.34411199,  0.9097930,
        -0.53677237,  1.63487183,  0.07272647, -0.70552658,  0.23083179,
        -1.31014264,  0.66858958,  0.48717484, -3.41804233,  0.99484487,
         0.62353267,  1.7368257 , -2.51836522,  0.33305551,  1.7699659;
    MatrixN b(1,5);
    b << -0.09637528,  0.23096641,  0.12591043,  0.06687821, -0.91183218;
    MatrixN y(2,3*5); // (N, T*M)
    y << -1.24134016, -0.61097456, -0.91085174, -3.9100686 ,  0.80761607,
         -0.30706023, -0.62484764,  3.84057832,  1.67364817, -3.79704162,
         -0.55824027,  0.10706638,  0.58436712, -0.77619534, -1.03265294,

          1.30911123, -1.26639562,  1.88829062,  3.80544112, -2.88742059,
         -2.55074087, -1.08604371,  5.00076914, -2.57243587, -3.92256111,
          0.6055915 , -4.37403498,  1.98858468,  3.0787865 , -4.12378799;

     TemporalAffine pe(R"({"inputShape":[4,3],"M":5})"_json);  // T=3;D=4; 12=T*D
    *(pe.params["W"])=W;
    *(pe.params["b"])=b;
    t_cppl states;
    MatrixN y0=pe.forward(x, nullptr, &states);
    return matCompT(y,y0,"TemporalAffineForward",eps,verbose);
}

bool checkTemporalAffineBackward(float eps=CP_DEFAULT_NUM_EPS, int verbose=1) {
    // N, T, D, M = 2, 3, 4, 5
    MatrixN x(2,3*4);  // (N, T*D)
    x << -0.7461612 , -1.55144546,  1.82876302,  0.5294384,
          0.64588784,  1.28880436, -1.13547596, -1.49195431,
         -0.12854723,  0.02965153,  0.27540986, -0.16085281,

          0.9820497 ,  0.17045352, -1.58826521, -0.75047715,
         -0.44713369,  1.18263892,  0.51718481, -1.91610422,
         -0.6941147 , -1.27365114, -0.44673534, -1.04071134;
    MatrixN W(4,5); // (D,M)
    W << -0.11804466,  0.60008559,  0.64535532, -1.34411199,  0.9097930,
        -0.53677237,  1.63487183,  0.07272647, -0.70552658,  0.23083179,
        -1.31014264,  0.66858958,  0.48717484, -3.41804233,  0.99484487,
         0.62353267,  1.7368257 , -2.51836522,  0.33305551,  1.7699659;
    MatrixN b(1,5);
    b << -0.09637528,  0.23096641,  0.12591043,  0.06687821, -0.91183218;
    MatrixN y(2,3*5); // (N, T*M)
    y << -1.24134016, -0.61097456, -0.91085174, -3.9100686 ,  0.80761607,
         -0.30706023, -0.62484764,  3.84057832,  1.67364817, -3.79704162,
         -0.55824027,  0.10706638,  0.58436712, -0.77619534, -1.03265294,

          1.30911123, -1.26639562,  1.88829062,  3.80544112, -2.88742059,
         -2.55074087, -1.08604371,  5.00076914, -2.57243587, -3.92256111,
          0.6055915 , -4.37403498,  1.98858468,  3.0787865 , -4.12378799;

     TemporalAffine pe(R"({"inputShape":[4,3],"M":5})"_json);  // T=3;D=4;12=T*D, M=5
    *(pe.params["W"])=W;
    *(pe.params["b"])=b;
    t_cppl cache;
    t_cppl states;
    MatrixN y0=pe.forward(x, &cache, &states);

    MatrixN dx(2,3*4);
    dx << -3.98456076,  -8.08466677, -10.98578141,  -5.24138688,
           0.22149385,   0.35130136,  -0.92980791,   0.1392131,
          -0.46269609,   0.62604295,  -1.37580256,   0.03723776,

           0.71871108,  -1.41956731,   3.26066682,  -6.19821418,
           0.83919535,   0.62097009,   4.20262083,   3.38660663,
          -0.38954197,  -1.41253428,  -2.45415827,   6.17485966;
    MatrixN dW(4,5);
    dW << -1.34217907,  2.02018413,  1.84852028, -2.80448865, -2.81008126,
       -3.66688543,  6.23758417, -1.04163871, -5.64704509, -3.32964834,
        1.50638603, -4.72100215, -0.13201567,  5.41395045,  2.38929841,
       -0.89459742, -0.77936534,  3.71094687,  4.05376848, -0.76022841;
    MatrixN db(1,5);
    db << 2.41756125, -4.69466384, -0.43658263,  1.08861002,  1.96764153;
    MatrixN dchain(2,15);
    dchain << 2.10790832, -3.50494649,  0.85018549,  2.00685021,  0.5674853,
          1.22151635,  0.59387383,  0.3102907 , -0.16995239, -0.46095165,
         -1.60655573,  0.31897037,  0.50353471,  1.36593723,  0.73342271,

         -0.11640313, -1.39148743,  0.7655294 , -1.29438842, -0.76265654,
         -0.35433334, -0.26196855, -1.59524514, -1.29404341,  0.26899792,
          1.16542877, -0.44910557, -1.27087778,  0.4742068 ,  1.6213438;

    t_cppl grads;
    MatrixN dx0=pe.backward(dchain, &cache, &states, &grads);
    bool allOk=true;
    bool ret=matCompT(dx,dx0,"TemporalAffineBackward dx",eps,verbose);
    if (!ret) allOk=false;
    ret=matCompT(dW,*(grads["W"]),"TemporalAffineBackward dW",eps,verbose);
    if (!ret) allOk=false;
    ret=matCompT(db,*(grads["b"]),"TemporalAffineBackward bx",eps,verbose);
    if (!ret) allOk=false;
    cppl_delete(&cache);
    cppl_delete(&grads);
    return allOk;
}

bool testTemporalAffine(int verbose) {
    Color::Modifier lblue(Color::FG_LIGHT_BLUE);
    Color::Modifier def(Color::FG_DEFAULT);
	bool bOk=true;
	t_cppl s1;
	cerr << lblue << "TemporalAffine Layer: " << def << endl;
	TemporalAffine pct(R"({"inputShape":[6,5],"M":7})"_json); // T=5;D=6;30=T*D
	MatrixN xtt(10, 30);
	xtt.setRandom();
	bool res=pct.selfTest(xtt, &s1, CP_DEFAULT_NUM_H, CP_DEFAULT_NUM_EPS, verbose);
	registerTestResult("TemporalAffine", "Numerical gradient", res, "");
	if (!res) bOk = false;

	res=checkTemporalAffineForward(CP_DEFAULT_NUM_EPS, verbose);
	registerTestResult("TemporalAffine", "Forward (with test-data)", res, "");
	if (!res) bOk = false;

	res=checkTemporalAffineBackward(CP_DEFAULT_NUM_EPS, verbose);
	registerTestResult("TemporalAffine", "Backward (with test-data)", res, "");
	if (!res) bOk = false;
	return bOk;
}

#endif
