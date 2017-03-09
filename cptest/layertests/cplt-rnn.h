#ifndef _CPLT_RNN_H
#define _CPLT_RNN_H

#include "../testneural.h"

bool checkRNNStepForward(floatN eps=CP_DEFAULT_NUM_EPS, int verbose=1) {
    MatrixN x(3,10);     // N, D, H = 3, 10, 4
    x << -0.4       , -0.36206897, -0.32413793, -0.2862069 , -0.24827586,
        -0.21034483, -0.17241379, -0.13448276, -0.09655172, -0.05862069,
        -0.02068966,  0.01724138,  0.05517241,  0.09310345,  0.13103448,
         0.16896552,  0.20689655,  0.24482759,  0.28275862,  0.32068966,
         0.35862069,  0.39655172,  0.43448276,  0.47241379,  0.51034483,
         0.54827586,  0.5862069 ,  0.62413793,  0.66206897,  0.7;
    MatrixN Wxh(10,4);
    MatrixN Whh(4,4);
    Wxh << -0.1       , -0.07435897, -0.04871795, -0.02307692,
         0.0025641 ,  0.02820513,  0.05384615,  0.07948718,
         0.10512821,  0.13076923,  0.15641026,  0.18205128,
         0.20769231,  0.23333333,  0.25897436,  0.28461538,
         0.31025641,  0.33589744,  0.36153846,  0.38717949,
         0.41282051,  0.43846154,  0.46410256,  0.48974359,
         0.51538462,  0.54102564,  0.56666667,  0.59230769,
         0.61794872,  0.64358974,  0.66923077,  0.69487179,
         0.72051282,  0.74615385,  0.77179487,  0.7974359 ,
         0.82307692,  0.84871795,  0.87435897,  0.9;
    Whh << -0.3       , -0.23333333, -0.16666667, -0.1,
        -0.03333333,  0.03333333,  0.1       ,  0.16666667,
         0.23333333,  0.3       ,  0.36666667,  0.43333333,
         0.5       ,  0.56666667,  0.63333333,  0.7;
    MatrixN bh(1,4);
    bh << -2.00000000e-01,   2.77555756e-17,   2.00000000e-01,
         4.00000000e-01;
    MatrixN hn(3,4);
    hn << -0.58172089, -0.50182032, -0.41232771, -0.31410098,
         0.66854692,  0.79562378,  0.87755553,  0.92795967,
         0.97934501,  0.99144213,  0.99646691,  0.99854353;
    MatrixN h(3,4);
    h << -0.2       , -0.13636364, -0.07272727, -0.00909091,
        0.05454545,  0.11818182,  0.18181818,  0.24545455,
        0.30909091,  0.37272727,  0.43636364,  0.5;

    RNN rnn(R"({"name":"testrnn","inputShape":[10,1],"H":4,"N":3})"_json);
    *(rnn.params["Wxh"])= Wxh;
    *(rnn.params["Whh"])= Whh;
    *(rnn.params["bh"])=bh;
    t_cppl cache;
    t_cppl states;
    cppl_set(&states,"testrnn-h",new MatrixN(h));
    MatrixN hn0=rnn.forward_step(x, &cache, &states, 0);
    cppl_delete(&cache);
    cppl_delete(&states);
    return matCompT(hn,hn0,"RNNForwardStep",eps,verbose);
}

bool checkRNNStepBackward(float eps=CP_DEFAULT_NUM_EPS, int verbose=1) {
    MatrixN x(4,5);     // N, D, H = 4, 5, 6
    x << .12416358,  1.69843114,  0.26088349,  0.61809901,  1.29498102,
         1.04395433,  0.345587  ,  1.82379102,  0.67565083,  1.19980857,
        -0.48936451,  1.3795558 , -0.44039028, -0.98906461,  0.11292283,
         0.10746271, -0.82250932, -0.94761786, -2.23684026,  0.3908439;
    MatrixN Wxh(5,6);
    Wxh << -1.64795906,  1.55945931,  1.1616202 ,  0.22775023,  1.12451202,
         0.98383165,
         0.24272267, -0.40812982,  0.6039808 , -0.53279051,  0.66677079,
        -1.47248404,
        -2.08350468, -1.6813117 , -0.27377932, -0.07012035,  1.5234011 ,
         0.86024426,
         1.23121803,  0.51911896,  0.91276421,  0.20830086, -0.5131898 ,
         1.41621487,
        -0.63816806, -1.15880041,  1.24110808,  0.71208072, -0.26885643,
        -0.08518922;
    MatrixN Whh(6,6);
    Whh << 0.16756294, -1.00421206, -0.55025714,  0.9548442 , -2.18848119,
        -0.21309667,
         0.91026643,  1.28894004, -0.05304641,  0.69584409, -2.08627571,
        -1.38057313,
        -0.41090696, -0.26789992,  0.80390613,  1.5490817 , -0.43289072,
         3.17763158,
         0.54785261,  0.33228893,  0.33076176, -1.98406491, -0.08705545,
         1.33665719,
         0.18163407, -1.24020649,  0.86931067,  0.24373755, -0.637415  ,
        -0.49735824,
        -0.2467502 ,  0.28155415, -0.4062929 , -0.41010748, -0.33394647,
         0.99012546;
    MatrixN bh(1,6);
    bh <<  -2.24794308, -0.62115254,  0.3501973 , -0.01646389,  0.194781  ,
       -0.25476446;
    MatrixN h0(4,6);
    h0 << 0.20024643,  1.40404312,  0.90177809, -1.23836782,  0.45040351,
         0.30889824,
        -0.09773321,  0.95785224,  0.59524814,  0.37567379,  0.67724748,
        -0.17839568,
         0.82151731, -2.16273274, -0.09992387, -0.35646773,  0.45099308,
         0.82678519,
        -0.94719149,  0.33138745, -1.53005522, -0.35261536,  0.29428635,
         1.49343694;

    MatrixN dx(4,5);
    dx << 0.05355669,  0.00464208, -0.06477791, -0.01111023, -0.0360655 ,
         0.50184965,  0.34297347,  0.83113146, -0.27801017, -0.06762191,
        -0.72487278, -0.46041568,  0.21955765, -0.55215642, -0.81686803,
        -0.46403852, -0.13747293, -0.20508103,  0.07899213,  0.20784653;
    MatrixN dWxh(5,6);
    dWxh << 4.16418675e-04,  -5.13777667e-02,   3.24080758e-01,
          2.35841191e-02,   4.95674789e-01,  -2.33743709e-02,
         -2.05667097e-02,   1.41571185e-01,  -9.09284030e-01,
          5.45687584e-04,   3.77206048e-01,   2.37304013e-02,
         -6.98110649e-03,   3.90700222e-02,   2.96873214e-01,
          1.32621137e-02,   1.15021702e+00,  -2.59606027e-02,
         -1.65688000e-02,   2.45353911e-01,   6.66110961e-01,
         -5.87466851e-02,   8.93706625e-01,  -4.89838207e-02,
         -8.18931445e-03,  -4.55617030e-02,  -7.57228635e-02,
          4.23928761e-02,   5.01266495e-01,  -1.98823763e-02;
    MatrixN dWhh(6,6);
    dWhh << -0.00848549,  0.11776217, -0.53975722, -0.0203512 ,  0.18871361,
         0.02795506,
        -0.00581244, -0.02458641,  1.43203792,  0.0087548 ,  0.38935482,
        -0.10644723,
        -0.01606056,  0.1802694 ,  0.073881  , -0.03033707,  0.67807337,
        -0.01986724,
         0.00834544, -0.02302092,  0.23743425, -0.00472736,  0.27917948,
         0.00529606,
        -0.0025646 , -0.04454486, -0.29973302,  0.03047122,  0.26617567,
         0.0078099 ,
         0.00505057, -0.14811397, -0.55461388,  0.04787431, -0.4614312 ,
         0.02665341;
    MatrixN dbh(1,6);
    dbh << -0.00364437, -0.1164864 , -0.66639534,  0.06503021,  0.25166159,
        0.01873921;
    MatrixN dh0(4,6);
    dh0 << -0.03007664,  0.06901193, -0.05617264, -0.01267693, -0.03818692,
        -0.002447  ,
        -1.03171308, -1.08006156, -0.17614215, -0.11097753, -0.25979309,
        -0.19269291,
         0.36227216, -0.01487735, -0.395828  , -0.1925051 , -0.59349363,
         0.30134821,
         0.68991653,  0.40216012,  0.17646913, -0.07084702,  0.30209163,
         0.04048871;

    MatrixN dchain(4,6); // dnext_h
    dchain << -0.23699244,  0.87596245,  0.29952303, -0.51286232, -0.20223674,
        -0.43225121,
        -0.31857928, -3.06145789,  0.91069566,  0.28754103,  1.82427869,
        -0.49471658,
        -0.51559122, -0.02988897, -0.84745675,  0.04859102,  0.10035852,
         0.47915608,
         1.30290015, -0.72761588, -0.17990189,  1.2176583 , -0.42103704,
         0.61638331;
    RNN rnn(R"({"name":"test2rnn","inputShape":[5,1],"H":6,"N":4})"_json);
    *(rnn.params["Wxh"])=Wxh;
    *(rnn.params["Whh"])=Whh;
    *(rnn.params["bh"])=bh;
    t_cppl cache;
    t_cppl grads;
    t_cppl states;
    cppl_set(&states,"test2rnn-h",new MatrixN(h0));
    MatrixN y=rnn.forward_step(x, &cache, &states);
    MatrixN dx0=rnn.backward_step(dchain, &cache, &states, &grads);
    bool allOk=true;
    bool ret=matCompT(dx,dx0,"RNNStepBackward dx",eps,verbose);
    if (!ret) allOk=false;
    ret=matCompT(dWxh,*(grads["Wxh"]),"RNNStepBackward dWxh",eps,verbose);
    if (!ret) allOk=false;
    ret=matCompT(dWhh,*(grads["Whh"]),"RNNStepBackward dWhh",eps,verbose);
    if (!ret) allOk=false;
    ret=matCompT(dbh,*(grads["bh"]),"RNNStepBackward bh",eps,verbose);
    if (!ret) allOk=false;
    ret=matCompT(dh0,*(grads["test2rnn-h0"]),"RNNStepBackward h0",eps,verbose);
    if (!ret) allOk=false;
    cppl_delete(&cache);
    cppl_delete(&grads);
    cppl_delete(&states);
    return allOk;
}

bool checkRNNForward(floatN eps=CP_DEFAULT_NUM_EPS, int verbose=1) {
    MatrixN x(2,12);   // N, T, D, H = 2, 3, 4, 5
    x << -0.1       , -0.0826087 , -0.06521739, -0.04782609,
         -0.03043478, -0.01304348,  0.00434783,  0.02173913,
          0.03913043,  0.05652174,  0.07391304,  0.09130435,

          0.10869565,  0.12608696,  0.14347826,  0.16086957,
          0.17826087,  0.19565217,  0.21304348,  0.23043478,
          0.24782609,  0.26521739,  0.2826087 ,  0.3;
    MatrixN Wxh(4,5);
    MatrixN Whh(5,5);
    Wxh << -0.2       , -0.16842105, -0.13684211, -0.10526316, -0.07368421,
        -0.04210526, -0.01052632,  0.02105263,  0.05263158,  0.08421053,
         0.11578947,  0.14736842,  0.17894737,  0.21052632,  0.24210526,
         0.27368421,  0.30526316,  0.33684211,  0.36842105,  0.4;
    Whh << -0.4       , -0.37916667, -0.35833333, -0.3375    , -0.31666667,
        -0.29583333, -0.275     , -0.25416667, -0.23333333, -0.2125    ,
        -0.19166667, -0.17083333, -0.15      , -0.12916667, -0.10833333,
        -0.0875    , -0.06666667, -0.04583333, -0.025     , -0.00416667,
         0.01666667,  0.0375    ,  0.05833333,  0.07916667,  0.1;
    MatrixN bh(1,5);
    bh << -0.7, -0.5, -0.3, -0.1,  0.1;
    MatrixN h0(2,5);
    h0 << -0.3       , -0.25555556, -0.21111111, -0.16666667, -0.12222222,
        -0.07777778, -0.03333333,  0.01111111,  0.05555556,  0.1;
    MatrixN hn(2,15);
    hn << -0.42070749, -0.27279261, -0.11074945,  0.05740409,  0.22236251,
         -0.39525808, -0.22554661, -0.0409454 ,  0.14649412,  0.32397316,
         -0.42305111, -0.24223728, -0.04287027,  0.15997045,  0.35014525,

         -0.55857474, -0.39065825, -0.19198182,  0.02378408,  0.23735671,
         -0.27150199, -0.07088804,  0.13562939,  0.33099728,  0.50158768,
         -0.51014825, -0.30524429, -0.06755202,  0.17806392,  0.40333043;

//                                          D,T
    RNN rnn(R"({"name":"rnn3","inputShape":[4,3],"H":5,"N":2})"_json);
    *(rnn.params["Wxh"])= Wxh;
    *(rnn.params["Whh"])= Whh;
    *(rnn.params["bh"])=bh;
    //*(rnn.params)["ho"]=h0;
    t_cppl cache;
    t_cppl states;
    states["rnn3-h"] = new MatrixN(h0);
    MatrixN hn0=rnn.forward(x, &cache, &states, 0);
    cppl_delete(&cache);
    cppl_delete(&states);
    return matCompT(hn,hn0,"RNNForward",eps,verbose);
}

bool checkRNNBackward(float eps=CP_DEFAULT_NUM_EPS, int verbose=1) {
    MatrixN x(2,30);   // N, D, T, H = 2, 3, 10, 5
    x << 1.42672007, -0.31937729, -0.86722593,
         -0.3784268 ,  0.87693254,  0.2210269 ,
         -1.08824345, -0.64407614,  1.43759927,
          0.28193136,  0.90717448,  1.25334125,
          2.39574773, -0.53590331, -0.50877974,
          0.27627503, -1.54064355,  1.32773898,
          1.07066922,  0.47327451,  1.29784147,
          0.653452  ,  0.70564059, -0.5578014 ,
          0.91983003, -0.2737394 ,  1.35892918,
          0.14778309,  0.98816614, -0.45552004,

         -0.87659908, -0.13201754,  1.06957197,
         -1.73428816, -1.13787752, -0.32563472,
          0.3202495 ,  0.13322343, -0.23705645,
          0.58770287,  0.07230072,  0.46665993,
         -0.29843096,  1.1122521 , -1.04458566,
         -0.84414253,  1.29579053,  0.85666362,
          1.01854585, -1.18368473,  0.93243884,
         -1.04041553, -1.12849339, -1.16842107,
          0.73113213,  0.85136499, -0.75428242,
         -0.39766453, -0.80114799,  0.31273153;
    MatrixN Wxh(3,5);
    Wxh << -0.12927579,  0.02471624,  1.61513095,  1.3376178 , -0.30023422,
         0.18728971,  0.88365881, -1.86947193, -1.57685207, -1.88697211,
        -0.42622774,  0.24390712, -0.30269361, -0.82475517,  0.43530684;
    MatrixN Whh(5,5);
    Whh << -0.81872798,  1.75444781,  0.08371593,  0.31629015, -1.63821244,
         1.85686335,  0.93631439, -0.53924214,  2.10224397,  0.83180435,
        -0.96315752, -0.66511744, -0.51743317,  0.4169285 ,  1.24867564,
         1.11834023,  0.61104288, -0.44082918,  0.05147677, -0.14183355,
         1.95697013,  1.58587945,  0.42601047, -0.38505994,  0.5758465;
    MatrixN bh(1,5);
    bh <<  -0.10951104,  0.24564546, -0.04626534,  0.68796566, -0.39304148;
    MatrixN h0(2,5);
    h0 << -0.20544637, -2.39043536, -0.33572273, -1.18048126,  0.2297227 ,
         0.00381195,  0.03485593,  0.45290272,  0.79040557,  0.53839077;

    MatrixN dx(2,30);
    dx << 2.83128334e-01,  -1.54396850e-01,  -2.03125890e-01,
           6.94499354e-01,   3.01747775e+00,  -7.83933285e-01,
          -3.85263109e+00,   4.52928685e+00,   6.59063543e-01,
           6.76851234e-01,   3.60756930e+00,  -1.36185681e+00,
           2.27191281e-01,   1.56398512e+00,  -2.25846567e-01,
           2.90811795e+00,  -3.40271758e+00,  -1.94746068e+00,
           3.53118649e+00,  -2.14451558e+00,  -1.07039645e+00,
           1.09935009e+00,  -2.13232708e+00,  -4.05936258e-01,
          -3.89871631e-02,   7.22680234e-02,   6.76815753e-02,
          -6.16315132e-01,  -2.11488618e+00,   6.61024864e-01,

           4.46130762e-01,   7.74248100e-02,  -4.61603013e-01,
          -3.38102735e+00,   4.42030626e+00,   5.70856460e-01,
           4.12266541e-01,  -9.11480589e-01,  -4.26059339e-03,
           4.32154518e-01,  -4.01449803e-01,   1.16172226e-03,
          -2.04333431e-01,   1.02275556e-01,   9.03355839e-02,
           2.19409870e-01,  -3.08578310e-01,   6.37864318e-01,
          -8.13136499e-01,  -4.82807046e+00,   1.06850626e+00,
           1.46153717e+00,  -1.68080991e+00,  -2.90687490e-01,
          -6.82187205e-01,   7.73966451e-01,   4.48969735e-01,
          -5.41287668e-01,   3.53712331e-01,   1.45758858e-02;
    MatrixN dWxh(3,5);
    dWxh << 1.41100052,  2.30220308,  7.29105998,  1.65324031,  1.71677344,
        -2.28393701,  0.57435196,  3.80965606, -2.81654354, -4.2065373 ,
         2.74039663,  0.8240278 , -0.38057752,  2.63125111, -1.8409216;
    MatrixN dWhh(5,5);
    dWhh << -4.0249321 , -1.58481823, -2.12826938, -2.49071418,  5.26850575,
        -3.077585  , -1.63456328, -0.69028048, -3.26015838,  6.25998547,
         3.13983089,  1.40499983,  7.08516274,  3.32499551, -3.39536687,
        -0.81957002,  0.92643005,  6.71996282,  0.39861412,  3.64331512,
         2.55450033,  1.99612196,  0.10288017,  1.84617216, -1.71983855;
    MatrixN dbh(1,5);
    dbh << -0.43575312,  1.35447753, -2.15414294,  4.02873156, -0.87407764;
    MatrixN dh0(2,5);
    dh0 << 0.16711121,  0.31080126, -0.01686812,  0.00980601, -0.1593779 ,
         0.37557634,  1.14471162, -0.53727051,  0.46902922,  0.55879207;

    MatrixN dchain(2,50);
    dchain << -0.12362327, -0.5674421 ,  0.8745609 ,  0.99932555, -1.63738807,
         -0.1352751 , -1.2843702 ,  2.3930869 , -0.08575731, -0.91934747,
          0.47303269, -0.3042664 ,  0.38503377,  0.01363704,  0.80027367,
          0.35418033, -0.26913838,  1.0614795 ,  0.13038746, -1.94323539,
         -0.900087  ,  1.14850186,  0.29905769, -1.92197358, -0.20150144,
         -0.37826346,  0.54382004,  0.15650663,  1.40933799,  2.09452943,
          0.41538121, -1.56705624,  0.92238285, -0.48502851,  0.07744951,
         -1.06722454,  0.79639577, -1.0321276 ,  1.77888984,  1.17946414,
          0.33070105, -1.46128781, -1.5602019 ,  0.58954647, -1.37958465,
          1.3165491 ,  0.23054624, -0.60895932, -0.70316943,  1.47789572,

          0.69971466, -0.3180744 , -0.69633622, -0.14756601,  0.08137661,
         -0.62384849, -0.78764898, -2.25953134, -0.18996891, -0.79340542,
          0.84748535, -0.72047164,  0.28052885, -0.24627851,  0.98943121,
          0.00872582, -0.57133626,  0.24619932, -0.19455666, -1.29171457,
         -1.31839253,  0.17045328, -0.29057405, -0.09411389, -0.6696823 ,
          0.74507544, -0.21036976,  0.62691113,  0.12732844,  0.76161537,
          0.89503194,  0.50062137,  0.02643405, -0.89958641,  2.1985117 ,
         -0.05634623,  1.31969603,  0.6188371 , -1.31854328, -1.27689832,
         -1.37433812,  0.19777929, -0.91821752, -0.54890713, -0.72215917,
         -1.38988503, -0.43959209, -0.33943549,  1.15280127, -0.06320341;
//                        D,T
    RNN rnn(R"({"name":"rnn4","inputShape":[3,10],"H":5,"N":2})"_json);   //inputShape=D, hidden=H
    *(rnn.params["Wxh"])=Wxh;
    *(rnn.params["Whh"])=Whh;
    *(rnn.params["bh"])=bh;
    //*(rnn.params["ho"])=h0;

    t_cppl cache;
    t_cppl states;
    t_cppl grads;
    states["rnn4-h"] = new MatrixN(h0);
    MatrixN y=rnn.forward(x, &cache, &states);
    cppl_update(&states, "rnn4-h", &h0);
    MatrixN dx0=rnn.backward(dchain, &cache, &states, &grads);
    bool allOk=true;
    bool ret=matCompT(dx,dx0,"RNNBackward dx",eps,verbose);
    if (!ret) allOk=false;
    ret=matCompT(dWxh,*(grads["Wxh"]),"RNNBackward dWxh",eps,verbose);
    if (!ret) allOk=false;
    ret=matCompT(dWhh,*(grads["Whh"]),"RNNBackward dWhh",eps,verbose);
    if (!ret) allOk=false;
    ret=matCompT(dbh,*(grads["bh"]),"RNNBackward bh",eps,verbose);
    if (!ret) allOk=false;
    ret=matCompT(dh0,*(grads["rnn4-h0"]),"RNNBackward h0",eps,verbose);
    if (!ret) allOk=false;

    cppl_delete(&cache);
    cppl_delete(&grads);
    cppl_delete(&states);
    return allOk;
}

bool testRNN(int verbose) {
    Color::Modifier lblue(Color::FG_LIGHT_BLUE);
    Color::Modifier def(Color::FG_DEFAULT);
	bool bOk=true;
	t_cppl s1;
	cerr << lblue << "RNN Layer: " << def << endl;
	// Numerical gradient
    // RNN
	int rnnN = 4; // N=4, D=5, H=6, T=7
	MatrixN xrnn(rnnN, 5 * 7);
	t_cppl rnstates;
	MatrixN h0(rnnN, 6);
	xrnn.setRandom();
	h0.setRandom();
	rnstates["rnn-h"] = &h0;
	//                                     D,T
	RNN rnn(R"({"name":"rnn","inputShape":[5,7],"H":6,"N":4,"noVectorizationTests":true,"nohupdate":true})"_json);
	bool res=rnn.selfTest(xrnn, &rnstates, 1e-4, 1e-4, verbose);
	registerTestResult("RNN", "Numerical gradient", res, "");
	if (!res) bOk = false;

	res=checkRNNStepForward(CP_DEFAULT_NUM_EPS, verbose);
	registerTestResult("RNN", "StepForward (with test-data)", res, "");
	if (!res) bOk = false;

	res=checkRNNStepBackward(CP_DEFAULT_NUM_EPS, verbose);
	registerTestResult("RNN", "StepBackward (with test-data)", res, "");
	if (!res) bOk = false;

	res=checkRNNForward(CP_DEFAULT_NUM_EPS, verbose);
	registerTestResult("RNN", "Forward (with test-data)", res, "");
	if (!res) bOk = false;

	res=checkRNNBackward(CP_DEFAULT_NUM_EPS, verbose);
	registerTestResult("RNN", "Backward (with test-data)", res, "");
	if (!res) bOk = false;
	return bOk;
}

#endif
