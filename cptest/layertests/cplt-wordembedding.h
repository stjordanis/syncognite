#ifndef _CPLT_WORDEMBEDDING_H
#define _CPLT_WORDEMBEDDING_H

#include "../testneural.h"

bool checkWordEmbeddingForward(floatN eps=CP_DEFAULT_NUM_EPS) {
    MatrixN x(2,4);  // N, T, V, D = 2, 4, 5, 3
    x << 0, 3, 1, 2,
         2, 1, 0, 3;
    MatrixN W(5,3);
    W << 0.        ,  0.07142857,  0.14285714,
         0.21428571,  0.28571429,  0.35714286,
         0.42857143,  0.5       ,  0.57142857,
         0.64285714,  0.71428571,  0.78571429,
         0.85714286,  0.92857143,  1.;
    MatrixN y(2,12);
    y << 0.        ,  0.07142857,  0.14285714,
         0.64285714,  0.71428571,  0.78571429,
         0.21428571,  0.28571429,  0.35714286,
         0.42857143,  0.5       ,  0.57142857,

         0.42857143,  0.5       ,  0.57142857,
         0.21428571,  0.28571429,  0.35714286,
         0.        ,  0.07142857,  0.14285714,
         0.64285714,  0.71428571,  0.78571429;

    CpParams cp("{inputShape=[4];V=5;D=3}");
    WordEmbedding we(cp);
    *(we.params["W"])= W;
    t_cppl cache;
    t_cppl states;
    MatrixN y0=we.forward(x, &cache, &states, 0);
    cppl_delete(&cache);
    return matComp(y,y0,"WordEmbeddingForward",eps);
}


bool checkWordEmbeddingBackward(float eps=CP_DEFAULT_NUM_EPS) {
    MatrixN x(50,3);   // N, T, V, D = 50, 3, 5, 6
    x << 2, 2, 4,       2, 2, 3,       2, 2, 4,       4, 3, 3,       3, 2, 2,
       1, 4, 1,       3, 3, 3,       4, 4, 3,       1, 1, 4,       2, 2, 2,
       2, 0, 1,       0, 2, 4,       3, 3, 2,       2, 2, 1,       3, 4, 3,
       2, 0, 4,       0, 1, 1,       0, 2, 3,       0, 3, 3,       2, 1, 1,
       3, 3, 1,       2, 3, 2,       4, 0, 4,       4, 1, 2,       0, 2, 3,
       2, 4, 3,       3, 2, 1,       2, 2, 3,       0, 2, 4,       1, 0, 1,
       3, 0, 4,       1, 2, 1,       2, 2, 1,       2, 2, 3,       1, 4, 0,
       3, 1, 4,       3, 2, 0,       4, 3, 0,       3, 0, 2,       4, 1, 4,
       3, 0, 0,       0, 2, 0,       4, 0, 3,       0, 3, 1,       2, 4, 3,
       4, 3, 3,       3, 0, 1,       2, 1, 2,       3, 1, 3,       4, 2, 0;
    MatrixN W(5,6);
    W << 1.18430336,  1.31756768, -0.06487105,  0.1958007 ,  1.16720325, -1.34495095,
       -1.1158534 , -0.26445229, -1.27061146, -0.10336515,  1.45402224, -0.1962724 ,
        0.87099429,  2.71394073, -0.56817905,  0.2253223 ,  1.24964303, 1.39740524,
       -0.44248965,  0.51079833, -0.80662343,  2.64545322, -1.02161667, -1.59424371,
       -0.35918011,  0.95326196,  1.11169519,  0.86099973,  0.65080126, 0.51127197;
    MatrixN y(50,18);
    y << 0.87099429,  2.71394073, -0.56817905,  0.2253223 ,  1.24964303, 1.39740524,
         0.87099429,  2.71394073, -0.56817905,  0.2253223 ,  1.24964303, 1.39740524,
        -0.35918011,  0.95326196,  1.11169519,  0.86099973,  0.65080126, 0.51127197,

        0.87099429,  2.71394073, -0.56817905,  0.2253223 ,  1.24964303, 1.39740524,
         0.87099429,  2.71394073, -0.56817905,  0.2253223 ,  1.24964303, 1.39740524,
        -0.44248965,  0.51079833, -0.80662343,  2.64545322, -1.02161667, -1.59424371,

        0.87099429,  2.71394073, -0.56817905,  0.2253223 ,  1.24964303, 1.39740524,
         0.87099429,  2.71394073, -0.56817905,  0.2253223 ,  1.24964303, 1.39740524,
        -0.35918011,  0.95326196,  1.11169519,  0.86099973,  0.65080126, 0.51127197,

       -0.35918011,  0.95326196,  1.11169519,  0.86099973,  0.65080126, 0.51127197,
        -0.44248965,  0.51079833, -0.80662343,  2.64545322, -1.02161667, -1.59424371,
        -0.44248965,  0.51079833, -0.80662343,  2.64545322, -1.02161667, -1.59424371,

       -0.44248965,  0.51079833, -0.80662343,  2.64545322, -1.02161667, -1.59424371,
         0.87099429,  2.71394073, -0.56817905,  0.2253223 ,  1.24964303, 1.39740524,
         0.87099429,  2.71394073, -0.56817905,  0.2253223 ,  1.24964303, 1.39740524,

       -1.1158534 , -0.26445229, -1.27061146, -0.10336515,  1.45402224, -0.1962724 ,
        -0.35918011,  0.95326196,  1.11169519,  0.86099973,  0.65080126, 0.51127197,
        -1.1158534 , -0.26445229, -1.27061146, -0.10336515,  1.45402224, -0.1962724 ,

       -0.44248965,  0.51079833, -0.80662343,  2.64545322, -1.02161667, -1.59424371,
        -0.44248965,  0.51079833, -0.80662343,  2.64545322, -1.02161667, -1.59424371,
        -0.44248965,  0.51079833, -0.80662343,  2.64545322, -1.02161667, -1.59424371,

       -0.35918011,  0.95326196,  1.11169519,  0.86099973,  0.65080126, 0.51127197,
        -0.35918011,  0.95326196,  1.11169519,  0.86099973,  0.65080126, 0.51127197,
        -0.44248965,  0.51079833, -0.80662343,  2.64545322, -1.02161667, -1.59424371,

       -1.1158534 , -0.26445229, -1.27061146, -0.10336515,  1.45402224, -0.1962724 ,
        -1.1158534 , -0.26445229, -1.27061146, -0.10336515,  1.45402224, -0.1962724 ,
        -0.35918011,  0.95326196,  1.11169519,  0.86099973,  0.65080126, 0.51127197,

        0.87099429,  2.71394073, -0.56817905,  0.2253223 ,  1.24964303, 1.39740524,
         0.87099429,  2.71394073, -0.56817905,  0.2253223 ,  1.24964303, 1.39740524,
         0.87099429,  2.71394073, -0.56817905,  0.2253223 ,  1.24964303, 1.39740524,

        0.87099429,  2.71394073, -0.56817905,  0.2253223 ,  1.24964303, 1.39740524,
         1.18430336,  1.31756768, -0.06487105,  0.1958007 ,  1.16720325, -1.34495095,
        -1.1158534 , -0.26445229, -1.27061146, -0.10336515,  1.45402224, -0.1962724 ,

        1.18430336,  1.31756768, -0.06487105,  0.1958007 ,  1.16720325, -1.34495095,
         0.87099429,  2.71394073, -0.56817905,  0.2253223 ,  1.24964303, 1.39740524,
        -0.35918011,  0.95326196,  1.11169519,  0.86099973,  0.65080126, 0.51127197,

       -0.44248965,  0.51079833, -0.80662343,  2.64545322, -1.02161667, -1.59424371,
        -0.44248965,  0.51079833, -0.80662343,  2.64545322, -1.02161667, -1.59424371,
         0.87099429,  2.71394073, -0.56817905,  0.2253223 ,  1.24964303, 1.39740524,

        0.87099429,  2.71394073, -0.56817905,  0.2253223 ,  1.24964303, 1.39740524,
         0.87099429,  2.71394073, -0.56817905,  0.2253223 ,  1.24964303, 1.39740524,
        -1.1158534 , -0.26445229, -1.27061146, -0.10336515,  1.45402224, -0.1962724 ,

       -0.44248965,  0.51079833, -0.80662343,  2.64545322, -1.02161667, -1.59424371,
        -0.35918011,  0.95326196,  1.11169519,  0.86099973,  0.65080126, 0.51127197,
        -0.44248965,  0.51079833, -0.80662343,  2.64545322, -1.02161667, -1.59424371,

        0.87099429,  2.71394073, -0.56817905,  0.2253223 ,  1.24964303, 1.39740524,
         1.18430336,  1.31756768, -0.06487105,  0.1958007 ,  1.16720325, -1.34495095,
        -0.35918011,  0.95326196,  1.11169519,  0.86099973,  0.65080126, 0.51127197,

        1.18430336,  1.31756768, -0.06487105,  0.1958007 ,  1.16720325, -1.34495095,
        -1.1158534 , -0.26445229, -1.27061146, -0.10336515,  1.45402224, -0.1962724 ,
        -1.1158534 , -0.26445229, -1.27061146, -0.10336515,  1.45402224, -0.1962724 ,

        1.18430336,  1.31756768, -0.06487105,  0.1958007 ,  1.16720325, -1.34495095,
         0.87099429,  2.71394073, -0.56817905,  0.2253223 ,  1.24964303, 1.39740524,
        -0.44248965,  0.51079833, -0.80662343,  2.64545322, -1.02161667, -1.59424371,

        1.18430336,  1.31756768, -0.06487105,  0.1958007 ,  1.16720325, -1.34495095,
        -0.44248965,  0.51079833, -0.80662343,  2.64545322, -1.02161667, -1.59424371,
        -0.44248965,  0.51079833, -0.80662343,  2.64545322, -1.02161667, -1.59424371,

        0.87099429,  2.71394073, -0.56817905,  0.2253223 ,  1.24964303, 1.39740524,
        -1.1158534 , -0.26445229, -1.27061146, -0.10336515,  1.45402224, -0.1962724 ,
        -1.1158534 , -0.26445229, -1.27061146, -0.10336515,  1.45402224, -0.1962724 ,

       -0.44248965,  0.51079833, -0.80662343,  2.64545322, -1.02161667, -1.59424371,
        -0.44248965,  0.51079833, -0.80662343,  2.64545322, -1.02161667, -1.59424371,
        -1.1158534 , -0.26445229, -1.27061146, -0.10336515,  1.45402224, -0.1962724 ,

        0.87099429,  2.71394073, -0.56817905,  0.2253223 ,  1.24964303, 1.39740524,
        -0.44248965,  0.51079833, -0.80662343,  2.64545322, -1.02161667, -1.59424371,
         0.87099429,  2.71394073, -0.56817905,  0.2253223 ,  1.24964303, 1.39740524,

       -0.35918011,  0.95326196,  1.11169519,  0.86099973,  0.65080126, 0.51127197,
         1.18430336,  1.31756768, -0.06487105,  0.1958007 ,  1.16720325, -1.34495095,
        -0.35918011,  0.95326196,  1.11169519,  0.86099973,  0.65080126, 0.51127197,

       -0.35918011,  0.95326196,  1.11169519,  0.86099973,  0.65080126, 0.51127197,
        -1.1158534 , -0.26445229, -1.27061146, -0.10336515,  1.45402224, -0.1962724 ,
         0.87099429,  2.71394073, -0.56817905,  0.2253223 ,  1.24964303, 1.39740524,

        1.18430336,  1.31756768, -0.06487105,  0.1958007 ,  1.16720325, -1.34495095,
         0.87099429,  2.71394073, -0.56817905,  0.2253223 ,  1.24964303, 1.39740524,
        -0.44248965,  0.51079833, -0.80662343,  2.64545322, -1.02161667, -1.59424371,

        0.87099429,  2.71394073, -0.56817905,  0.2253223 ,  1.24964303, 1.39740524,
        -0.35918011,  0.95326196,  1.11169519,  0.86099973,  0.65080126, 0.51127197,
        -0.44248965,  0.51079833, -0.80662343,  2.64545322, -1.02161667, -1.59424371,

       -0.44248965,  0.51079833, -0.80662343,  2.64545322, -1.02161667, -1.59424371,
         0.87099429,  2.71394073, -0.56817905,  0.2253223 ,  1.24964303, 1.39740524,
        -1.1158534 , -0.26445229, -1.27061146, -0.10336515,  1.45402224, -0.1962724 ,

        0.87099429,  2.71394073, -0.56817905,  0.2253223 ,  1.24964303, 1.39740524,
         0.87099429,  2.71394073, -0.56817905,  0.2253223 ,  1.24964303, 1.39740524,
        -0.44248965,  0.51079833, -0.80662343,  2.64545322, -1.02161667, -1.59424371,

        1.18430336,  1.31756768, -0.06487105,  0.1958007 ,  1.16720325, -1.34495095,
         0.87099429,  2.71394073, -0.56817905,  0.2253223 ,  1.24964303, 1.39740524,
        -0.35918011,  0.95326196,  1.11169519,  0.86099973,  0.65080126, 0.51127197,

       -1.1158534 , -0.26445229, -1.27061146, -0.10336515,  1.45402224, -0.1962724 ,
         1.18430336,  1.31756768, -0.06487105,  0.1958007 ,  1.16720325, -1.34495095,
        -1.1158534 , -0.26445229, -1.27061146, -0.10336515,  1.45402224, -0.1962724 ,

       -0.44248965,  0.51079833, -0.80662343,  2.64545322, -1.02161667, -1.59424371,
         1.18430336,  1.31756768, -0.06487105,  0.1958007 ,  1.16720325, -1.34495095,
        -0.35918011,  0.95326196,  1.11169519,  0.86099973,  0.65080126, 0.51127197,

       -1.1158534 , -0.26445229, -1.27061146, -0.10336515,  1.45402224, -0.1962724 ,
         0.87099429,  2.71394073, -0.56817905,  0.2253223 ,  1.24964303, 1.39740524,
        -1.1158534 , -0.26445229, -1.27061146, -0.10336515,  1.45402224, -0.1962724 ,

        0.87099429,  2.71394073, -0.56817905,  0.2253223 ,  1.24964303, 1.39740524,
         0.87099429,  2.71394073, -0.56817905,  0.2253223 ,  1.24964303, 1.39740524,
        -1.1158534 , -0.26445229, -1.27061146, -0.10336515,  1.45402224, -0.1962724 ,

        0.87099429,  2.71394073, -0.56817905,  0.2253223 ,  1.24964303, 1.39740524,
         0.87099429,  2.71394073, -0.56817905,  0.2253223 ,  1.24964303, 1.39740524,
        -0.44248965,  0.51079833, -0.80662343,  2.64545322, -1.02161667, -1.59424371,

       -1.1158534 , -0.26445229, -1.27061146, -0.10336515,  1.45402224, -0.1962724 ,
        -0.35918011,  0.95326196,  1.11169519,  0.86099973,  0.65080126, 0.51127197,
         1.18430336,  1.31756768, -0.06487105,  0.1958007 ,  1.16720325, -1.34495095,

       -0.44248965,  0.51079833, -0.80662343,  2.64545322, -1.02161667, -1.59424371,
        -1.1158534 , -0.26445229, -1.27061146, -0.10336515,  1.45402224, -0.1962724 ,
        -0.35918011,  0.95326196,  1.11169519,  0.86099973,  0.65080126, 0.51127197,

       -0.44248965,  0.51079833, -0.80662343,  2.64545322, -1.02161667, -1.59424371,
         0.87099429,  2.71394073, -0.56817905,  0.2253223 ,  1.24964303, 1.39740524,
         1.18430336,  1.31756768, -0.06487105,  0.1958007 ,  1.16720325, -1.34495095,

       -0.35918011,  0.95326196,  1.11169519,  0.86099973,  0.65080126, 0.51127197,
        -0.44248965,  0.51079833, -0.80662343,  2.64545322, -1.02161667, -1.59424371,
         1.18430336,  1.31756768, -0.06487105,  0.1958007 ,  1.16720325, -1.34495095,

       -0.44248965,  0.51079833, -0.80662343,  2.64545322, -1.02161667, -1.59424371,
         1.18430336,  1.31756768, -0.06487105,  0.1958007 ,  1.16720325, -1.34495095,
         0.87099429,  2.71394073, -0.56817905,  0.2253223 ,  1.24964303, 1.39740524,

       -0.35918011,  0.95326196,  1.11169519,  0.86099973,  0.65080126, 0.51127197,
        -1.1158534 , -0.26445229, -1.27061146, -0.10336515,  1.45402224, -0.1962724 ,
        -0.35918011,  0.95326196,  1.11169519,  0.86099973,  0.65080126, 0.51127197,

       -0.44248965,  0.51079833, -0.80662343,  2.64545322, -1.02161667, -1.59424371,
         1.18430336,  1.31756768, -0.06487105,  0.1958007 ,  1.16720325, -1.34495095,
         1.18430336,  1.31756768, -0.06487105,  0.1958007 ,  1.16720325, -1.34495095,

        1.18430336,  1.31756768, -0.06487105,  0.1958007 ,  1.16720325, -1.34495095,
         0.87099429,  2.71394073, -0.56817905,  0.2253223 ,  1.24964303, 1.39740524,
         1.18430336,  1.31756768, -0.06487105,  0.1958007 ,  1.16720325, -1.34495095,

       -0.35918011,  0.95326196,  1.11169519,  0.86099973,  0.65080126, 0.51127197,
         1.18430336,  1.31756768, -0.06487105,  0.1958007 ,  1.16720325, -1.34495095,
        -0.44248965,  0.51079833, -0.80662343,  2.64545322, -1.02161667, -1.59424371,

        1.18430336,  1.31756768, -0.06487105,  0.1958007 ,  1.16720325, -1.34495095,
        -0.44248965,  0.51079833, -0.80662343,  2.64545322, -1.02161667, -1.59424371,
        -1.1158534 , -0.26445229, -1.27061146, -0.10336515,  1.45402224, -0.1962724 ,

        0.87099429,  2.71394073, -0.56817905,  0.2253223 ,  1.24964303, 1.39740524,
        -0.35918011,  0.95326196,  1.11169519,  0.86099973,  0.65080126, 0.51127197,
        -0.44248965,  0.51079833, -0.80662343,  2.64545322, -1.02161667, -1.59424371,

       -0.35918011,  0.95326196,  1.11169519,  0.86099973,  0.65080126, 0.51127197,
        -0.44248965,  0.51079833, -0.80662343,  2.64545322, -1.02161667, -1.59424371,
        -0.44248965,  0.51079833, -0.80662343,  2.64545322, -1.02161667, -1.59424371,

       -0.44248965,  0.51079833, -0.80662343,  2.64545322, -1.02161667, -1.59424371,
         1.18430336,  1.31756768, -0.06487105,  0.1958007 ,  1.16720325, -1.34495095,
        -1.1158534 , -0.26445229, -1.27061146, -0.10336515,  1.45402224, -0.1962724 ,

        0.87099429,  2.71394073, -0.56817905,  0.2253223 ,  1.24964303, 1.39740524,
        -1.1158534 , -0.26445229, -1.27061146, -0.10336515,  1.45402224, -0.1962724 ,
         0.87099429,  2.71394073, -0.56817905,  0.2253223 ,  1.24964303, 1.39740524,

       -0.44248965,  0.51079833, -0.80662343,  2.64545322, -1.02161667, -1.59424371,
        -1.1158534 , -0.26445229, -1.27061146, -0.10336515,  1.45402224, -0.1962724 ,
        -0.44248965,  0.51079833, -0.80662343,  2.64545322, -1.02161667, -1.59424371,

       -0.35918011,  0.95326196,  1.11169519,  0.86099973,  0.65080126, 0.51127197,
         0.87099429,  2.71394073, -0.56817905,  0.2253223 ,  1.24964303, 1.39740524,
         1.18430336,  1.31756768, -0.06487105,  0.1958007 ,  1.16720325, -1.34495095;

    MatrixN dW(5,6);
    dW << 6.57703128,  -2.5158619 ,  -2.4048323 ,  -0.56046221, 1.9801882 ,  -7.03243431,
        -1.84139006,   5.53064019,  -7.90375968,  -4.15784493, -2.30048648, -10.60339139,
        -5.55600265,  -0.9809074 ,  11.56072571,  -1.1762269 , 14.89790292,  -7.59683227,
         0.02152891,  -0.98163116, -11.09325249,  12.89208802, -1.65953027,  -3.5457068 ,
         1.43047737, -10.66973898,  -4.09373739,   2.97128051, 5.48698808,  -2.42671394;

    MatrixN dchain(50,18);
    dchain << -4.31155985e-01,   3.04636116e-01,  -5.76036452e-01,
           1.34346217e+00,  -1.56977803e+00,   9.53062177e-01,
         -2.32527497e+00,  -1.91471618e+00,  -2.23224566e-01,
          -2.79988843e-01,  -7.34309976e-01,   7.37927616e-01,
         -3.81669556e-01,   3.07820528e-01,  -6.87853455e-01,
          -1.61981965e+00,   5.85127848e-02,   3.78730241e-01,

         1.90289061e-01,  -1.55883636e-02,   8.04767034e-01,
           8.29089195e-01,   1.29019073e+00,  -5.63624744e-01,
         -2.33491388e-01,   1.52531133e-01,  -4.42511101e-02,
          -1.39082483e+00,   2.11108474e+00,  -2.58427740e-01,
         -4.47108702e-01,   7.56842126e-02,   1.49815291e+00,
          -5.39383034e-01,   1.02081198e+00,  -2.36163700e-01,

         2.02371465e+00,  -2.34775058e+00,  -6.06310613e-01,
          -1.68303430e+00,   5.60491549e-01,   5.27199086e-01,
          5.74884404e-01,   5.98690055e-01,   9.25041080e-01,
           7.87760208e-01,  -1.36248721e-01,  -8.53732031e-01,
         -8.31088195e-01,   6.26089578e-01,   2.64855320e-01,
           1.12364711e-01,  -9.13073760e-01,   1.27975795e+00,

         1.00601557e-02,  -1.98668877e+00,  -5.57599685e-01,
           4.78387318e-01,  -9.76566817e-01,  -9.18192710e-01,
          9.06480642e-01,  -2.24908254e-01,  -5.33321669e-01,
          -4.81308512e-01,  -2.26368467e+00,   7.89044520e-01,
         -1.18255292e+00,   5.58188271e-01,   9.81720080e-01,
           1.47957999e+00,   1.97878002e-01,   1.12246513e+00,

        -6.59341773e-01,   7.63327393e-03,  -1.52244908e-01,
          -3.60423371e-01,  -1.08592186e+00,   9.17466891e-01,
          2.68432999e-01,   1.27750788e+00,   9.15166611e-01,
           9.57232438e-01,  -2.37903705e-01,  -1.39957238e+00,
         -1.67472236e-01,   1.61770940e+00,   1.49172783e+00,
           1.19321012e+00,   1.56968395e+00,  -3.19518134e-01,

         6.80717138e-01,  -6.67328846e-01,  -3.27716535e-01,
          -1.56949245e+00,   2.45367186e-01,  -1.55173667e-01,
          2.88742508e+00,   4.36435583e-01,  -6.17818659e-01,
           7.58711394e-01,  -1.13301868e+00,  -1.74221495e+00,
         -2.56915283e-01,   7.21472851e-01,  -1.93810128e+00,
          -9.38594675e-02,  -5.44652561e-01,  -1.94294431e+00,

        -4.62143598e-01,   1.37253967e-01,  -2.19816209e+00,
           6.47737672e-01,  -7.82904954e-01,  -1.04598675e-01,
         -1.14095200e+00,   3.49037530e-01,  -1.44708129e-01,
          -3.92119785e-01,  -4.04236484e-02,  -2.42980254e+00,
          2.08226455e-01,  -2.90385801e-01,  -1.27331429e+00,
           1.84686068e+00,   9.30755617e-01,  -2.35213452e+00,

         1.10188719e+00,  -8.63828523e-01,   1.43500050e+00,
          -1.63094860e-01,  -1.22637523e+00,  -1.91542876e+00,
          4.25101830e-02,  -5.87506970e-02,  -1.32045131e-01,
          -2.52622917e-01,  -8.48834712e-02,  -2.75771951e-01,
         -4.82452528e-01,  -1.38499456e+00,  -1.30047378e+00,
          -3.14372319e-01,  -6.56342465e-01,  -2.86317281e-01,

        -5.01194680e-01,   1.77994514e+00,  -7.28210954e-01,
          -7.33354324e-01,   3.08099251e-02,  -1.74096074e+00,
          1.15229675e-01,  -7.02812892e-01,   2.35613995e-01,
          -7.93552931e-02,  -4.70763126e-01,  -9.43951979e-01,
          2.59696787e-01,   7.01485180e-01,  -4.42082725e-01,
           1.92474471e-01,   1.99831158e+00,  -4.09887202e-01,

        -1.60099700e+00,  -1.71078217e+00,  -2.10558049e-01,
          -7.86149055e-01,   8.11518314e-01,  -1.29208361e+00,
          2.33709652e-01,  -8.99147388e-01,   2.10547794e-01,
          -1.68970891e-01,   2.33205059e-01,   6.74205734e-01,
         -1.64560083e+00,  -9.56919391e-01,   1.21230116e-01,
          -8.17079918e-01,  -1.76318025e-01,   2.00245174e+00,

         9.20730450e-01,  -1.30244925e+00,   1.22923051e+00,
          -1.23278978e-01,   1.53396985e+00,   1.31861844e+00,
         -7.01539987e-02,  -7.55039499e-01,   3.74019452e-01,
           3.46426908e-01,   5.90105281e-01,  -4.90502179e-01,
         -8.77754481e-01,  -4.66997383e-01,  -1.14501742e+00,
          -1.98899308e+00,   7.95462750e-01,  -7.88303110e-01,

        -1.01827272e+00,   2.17948816e+00,  -1.63726171e+00,
          -6.29299177e-01,   3.83652836e-02,  -9.69518947e-01,
          1.98857448e+00,   5.01936554e-01,  -4.24739386e-01,
           5.09527578e-01,   2.25761862e+00,  -1.24957199e-01,
         -2.48292588e-01,  -6.34693387e-01,  -9.73808602e-01,
          -8.57785211e-01,  -6.51783398e-01,   8.88913291e-01,

        -8.69043854e-01,  -1.18082282e+00,  -1.00065755e+00,
           1.30612560e+00,  -7.35031228e-01,   3.51137480e-01,
          4.71057143e-01,   5.12555515e-01,  -1.24871457e+00,
          -3.79658033e-01,   1.53845868e+00,  -2.05896678e-01,
          2.42117705e-01,   4.51551558e-01,  -1.10265006e-01,
          -7.91071177e-01,  -4.60139033e-01,  -1.64217486e+00,

         4.16404343e-01,  -7.99662060e-02,   3.16352483e-01,
           1.24280766e+00,  -3.78544912e-01,   1.07051918e+00,
          1.45119245e+00,   1.60576962e+00,  -5.16557790e-02,
          -5.19180833e-01,   3.33977606e-01,  -2.78177114e-01,
          2.46509043e-01,  -1.60257957e+00,  -5.85733961e-01,
          -1.72968151e+00,   7.08844788e-01,  -5.55529974e-01,

        -1.18041168e+00,  -1.77204146e+00,   1.70359432e+00,
           7.81898343e-01,   1.39156902e-02,  -2.38538499e+00,
         -5.33143019e-01,  -3.59604610e-01,   6.01056947e-01,
           1.68336988e+00,   1.71382882e+00,   1.55079778e+00,
         -1.63876737e-01,  -2.65244545e-01,  -1.33640849e+00,
           5.34370484e-01,   1.19901242e+00,  -6.02268724e-01,

        -2.45733248e-01,  -1.11441850e+00,  -3.74565789e-01,
           1.02722478e+00,  -5.00240737e-01,  -5.47721665e-01,
          3.43180157e-01,  -1.55243231e+00,  -6.04782011e-01,
           2.93012617e-01,   1.52629182e-01,  -1.47540996e+00,
          1.39324638e+00,  -8.71598140e-01,  -1.07690997e+00,
           7.59412602e-01,   2.09194012e+00,  -2.35064032e-01,

        -6.17372383e-01,   5.83530438e-01,  -8.80439192e-01,
           1.63966294e+00,   1.62337292e+00,  -9.10417218e-01,
          1.50690327e-01,   3.75330517e-01,   3.73452502e-01,
           1.09048308e+00,   1.59408559e+00,  -6.15533305e-01,
         -1.81834987e+00,   6.60315264e-02,   1.12634877e+00,
          -1.23865268e-02,  -1.13301253e+00,   4.44399499e-01,

         2.85169082e+00,  -3.25845406e-01,  -1.17709389e+00,
          -5.00034576e-01,   3.02898730e-01,   2.65687401e+00,
         -3.00545865e-01,  -4.15632556e-02,   1.20526914e+00,
           1.86551716e+00,   1.80932662e+00,  -1.08486197e+00,
          7.05953120e-01,  -8.93633065e-01,   1.03929389e+00,
           2.48829528e-01,   2.22148469e+00,  -5.85659948e-01,

         4.85048540e-01,   1.95669064e-01,   2.30574640e-01,
           2.43485955e-01,   3.20092286e-01,  -3.88626678e-01,
         -5.18286839e-01,  -2.99327823e-01,  -6.62539167e-01,
           3.60854916e-01,  -3.12667827e+00,   8.62987225e-01,
          7.27701476e-03,  -1.24710380e+00,   8.72597138e-01,
           2.33068982e+00,   3.54560475e-01,   8.08633044e-01,

         1.04326507e-01,   9.96614805e-02,   1.56206420e+00,
          -6.91838304e-01,  -4.38314048e-01,  -5.60759297e-01,
          5.96399408e-01,   6.87212410e-01,   4.27442042e-01,
           7.09631094e-01,  -3.03716330e-01,  -1.64813415e+00,
          4.63246890e-01,  -9.58905474e-01,  -7.41542084e-02,
           7.53824848e-01,  -6.45040571e-01,  -1.04761720e+00,

        -7.97772043e-01,   7.56043633e-01,  -3.13538952e-01,
          -3.27113755e-01,  -5.34633671e-01,  -1.91239022e+00,
         -6.86930292e-01,   1.14497282e+00,  -1.27122705e-01,
          -1.37972925e+00,  -6.95752397e-01,   2.29591352e+00,
          7.43228271e-01,  -1.57762341e+00,  -1.18439019e-01,
           5.60208717e-01,  -1.83815984e-01,  -5.13840852e-01,

         4.80426739e-01,   2.57299970e-01,  -1.51983379e+00,
           5.91255282e-01,  -1.86778892e-01,   8.44151108e-01,
          4.89950721e-01,  -3.43092457e-01,  -7.99817978e-01,
           4.45890222e-01,  -5.33600821e-01,  -4.12436211e-01,
          7.55738416e-01,   2.65854528e-01,  -1.26638674e+00,
           6.15897300e-01,  -2.08655573e+00,   1.06886231e+00,

        -8.01182027e-01,  -1.36524223e+00,  -1.69508564e+00,
           7.15379996e-01,  -1.02324027e+00,   1.58388116e+00,
          9.39420468e-01,   1.00690727e+00,  -1.47562968e+00,
           5.99650449e-01,  -8.96991965e-01,   4.04578969e-01,
         -1.40572648e+00,   3.28092042e-01,   7.99283792e-01,
          -2.00610783e-01,   2.18081645e+00,   6.07040833e-01,

        -1.41067127e-01,  -2.89636363e+00,  -8.14486863e-01,
           1.11354820e+00,   1.08883180e+00,   3.29342459e-01,
          4.66141525e-01,  -9.41647420e-02,  -1.32366764e+00,
          -1.19784479e+00,  -5.74595698e-01,   1.44654042e+00,
         -2.34073098e-01,  -1.23510611e+00,   3.33662605e-01,
          -8.42515982e-01,  -9.27645805e-01,  -5.43829052e-01,

         1.66846974e+00,  -6.17165823e-01,   5.20159596e-01,
          -6.90550329e-01,  -5.53225066e-01,  -2.58081077e-01,
         -8.55913161e-01,  -2.59879714e+00,   1.85448811e+00,
          -1.65008497e+00,   2.86250911e-01,  -1.55193990e+00,
          7.97035277e-01,   8.03993399e-01,   1.58189731e+00,
          -1.04787223e+00,  -1.03317820e+00,   7.05538751e-01,

        -8.58641278e-01,  -7.51694530e-02,   3.15983282e-02,
          -1.48941466e+00,   1.87727238e+00,   4.45810803e-01,
          1.20476455e+00,   1.46131201e+00,  -1.02806399e+00,
          -1.00274660e+00,   2.37073636e-01,  -1.03836418e+00,
         -1.93235403e-01,   6.87801258e-01,  -8.90048204e-01,
          -1.56307597e+00,  -1.02969197e-01,   1.07914042e-01,

         1.09516309e+00,  -4.13849881e-01,  -1.31508580e+00,
           1.42043280e+00,   5.67935552e-01,  -7.59876869e-02,
         -1.14673619e-02,   1.03043727e+00,   1.00052554e+00,
           2.74062274e-01,   5.79066586e-02,   1.07412225e-01,
          1.18430114e+00,  -1.55253963e-01,   1.66042019e+00,
          -1.28020907e-01,  -1.45891693e+00,   1.13836616e-01,

        -1.76449390e+00,   5.59409958e-01,   1.53815988e+00,
          -5.28984763e-01,   2.14618254e-01,  -3.81240902e-01,
         -7.20836145e-01,   2.00749846e+00,   1.06266899e+00,
          -1.91243152e+00,   2.24428758e+00,  -1.84409679e-01,
          5.87125633e-01,   8.58309020e-01,  -6.25143154e-01,
           2.78875376e-01,  -7.28348108e-01,   8.76485552e-01,

        -9.71892496e-01,   4.93841898e-01,  -5.31220360e-01,
           1.48819888e+00,  -6.38715876e-01,  -1.28166762e+00,
          1.07312775e+00,   1.72347992e+00,  -1.28664706e+00,
          -2.87010026e-02,  -6.85000229e-01,   9.14587829e-01,
          1.13514272e+00,  -1.62642377e+00,   6.32414478e-01,
           6.62588852e-01,  -6.35134787e-01,   7.85343454e-02,

        -1.76154743e-01,  -1.83328255e+00,  -8.53166674e-01,
          -6.66539525e-01,  -6.38514647e-01,  -1.40667093e-01,
          7.69376292e-01,  -1.40979261e-03,   2.07010626e+00,
          -6.25688429e-01,   4.83686014e-02,  -1.44576524e+00,
         -6.94438732e-01,   1.31752347e+00,   4.02502872e-01,
           4.33088146e-01,   2.87726384e-01,   1.66192612e+00,

         1.26162206e+00,  -8.96723746e-01,   8.35230254e-01,
           1.47025882e+00,   1.71444727e+00,  -1.03640153e-01,
         -8.79136396e-01,  -1.92114534e+00,   6.29989104e-02,
           1.56201374e+00,  -1.20923770e+00,  -1.32235992e+00,
         -7.00343744e-01,  -9.76031002e-01,  -4.42578184e-01,
           5.92290783e-01,   5.04465392e-01,  -3.14701445e-01,

        -2.70210054e+00,   3.13530562e-01,  -1.68982765e-01,
           5.03908530e-01,   4.08762574e-01,  -1.81977196e-02,
         -1.05873476e+00,  -4.17967001e-01,   1.16710891e+00,
          -2.00788229e+00,  -1.35538324e-01,  -1.19768922e+00,
         -3.44087208e-01,   5.06142503e-02,  -1.75154789e+00,
          -2.86670530e-01,  -3.06521392e-02,  -3.18090898e-02,

        -1.68178474e+00,   8.86468127e-01,  -1.03306690e+00,
           6.39593730e-01,   2.50691289e+00,  -3.06308787e-01,
         -7.96919999e-01,   8.93006061e-01,  -5.73288451e-01,
           3.00361732e-01,   2.79256662e-01,   2.97500409e-01,
         -9.81403355e-01,  -8.26994056e-03,   4.12317813e-01,
           9.30744436e-01,   1.73777926e-01,   2.16947574e-01,

        -1.31396150e+00,   9.02684556e-01,   8.64829920e-02,
          -1.09038751e+00,   3.37682535e-01,   2.83330036e-01,
          4.52457427e-01,  -2.00937621e-01,   4.76759775e-02,
          -4.45976623e-01,   1.45060397e-01,  -7.67886737e-01,
          5.67260067e-03,  -1.39369096e+00,   9.86038404e-03,
          -2.12737133e-01,   5.06914561e-02,  -9.75780800e-01,

         2.68497225e-01,   1.34625917e+00,  -1.58330136e+00,
          -3.29170732e-01,   8.97748995e-01,  -4.90358088e-01,
         -3.26167673e+00,   9.49439013e-01,   1.40708968e+00,
          -1.04728946e+00,   3.31646766e-01,   9.26756913e-01,
          1.02516536e-01,   1.38081940e+00,  -4.95855548e-01,
           2.42014772e-04,   2.34133845e-01,   4.58466128e-01,

        -1.66809941e+00,   5.88892636e-01,  -3.12137454e-01,
           1.48730246e-01,   1.42263147e+00,   1.19499367e-01,
         -8.54124348e-01,  -4.23469119e-01,   1.64702244e+00,
           1.54750829e+00,   7.39997107e-01,  -1.68630886e-01,
         -5.21372722e-02,  -3.08620934e-01,   4.22296503e-01,
           9.60835767e-01,   1.51067087e+00,  -5.46959842e-01,

        -1.40799506e+00,   5.07759445e-01,  -1.13270527e+00,
           9.50769408e-01,  -2.81832891e-01,   1.03635456e+00,
          5.34666859e-01,  -1.40730495e+00,  -8.09130724e-01,
           1.94701557e+00,   2.38326132e+00,   8.51644737e-01,
          1.86043655e+00,  -5.99178208e-01,  -3.75475302e-01,
          -1.16102567e+00,   1.22690094e+00,  -1.26601806e+00,

         1.67082777e+00,  -2.02325570e+00,  -9.18473614e-01,
          -6.86102628e-01,  -6.55003285e-01,  -7.37611899e-01,
          5.41049350e-01,   6.71355582e-01,  -8.46476232e-01,
           1.00316212e+00,   7.54535946e-01,  -7.74631571e-01,
          1.97348655e+00,  -1.18932731e-01,  -1.61503824e-01,
          -1.72299691e+00,  -1.14258003e+00,  -8.51547499e-01,

         1.08355337e+00,   9.33029423e-01,   7.37857535e-01,
           5.31297388e-01,  -2.99338721e-01,   1.10068392e+00,
         -7.69485619e-01,  -9.69027786e-02,   1.25405241e+00,
           1.37377197e+00,   1.44505481e+00,  -4.33491718e-01,
          2.04579425e+00,   1.10390315e+00,   8.54738530e-01,
           1.94192759e+00,   1.69539374e+00,  -1.26602577e+00,

        -4.57146673e-01,  -4.32562358e-01,  -2.81212058e-01,
          -9.61253115e-01,   7.09052725e-01,  -1.07568844e+00,
          1.21488628e+00,   2.34088395e+00,   1.18562004e+00,
           9.84519835e-02,  -5.69680149e-01,  -7.15518147e-01,
          4.96674060e-01,   1.30273540e-01,  -4.09324988e-01,
           7.39761886e-01,   1.32087135e+00,   3.77679567e-02,

         4.74987061e-01,  -2.02344888e+00,  -5.53750929e-01,
           1.57065275e-01,   3.84381608e-02,  -1.96494877e+00,
         -6.42659484e-01,  -8.25548291e-01,   5.02974462e-01,
          -1.28704546e-01,   6.01095830e-01,  -1.35059747e-01,
          4.71842519e-01,  -7.66394036e-01,  -2.57184489e-01,
          -1.98630312e+00,  -1.33835038e+00,   5.96336269e-01,

        -8.98934777e-01,  -8.32161183e-02,  -2.09237579e-01,
           6.90660304e-01,   8.47626846e-01,   1.01753613e-01,
         -1.39954884e+00,  -6.36478569e-01,   8.77917229e-01,
          -1.28850812e-01,  -1.50988394e+00,  -8.25078605e-01,
         -1.63744849e-01,  -1.39236187e+00,   7.45789972e-01,
          -5.82983563e-01,  -1.51873307e+00,  -5.35550506e-01,

         2.58524195e-01,  -1.90148884e+00,  -4.11906015e-01,
           1.07156531e+00,  -7.54493310e-01,  -1.16096069e+00,
         -2.20507568e-01,  -1.99413892e-01,  -5.97949055e-01,
           1.22513747e-01,  -7.58802739e-01,  -7.86321555e-01,
         -2.02968119e-01,   8.93187510e-01,  -3.56893065e-01,
           2.03121085e+00,   2.42405615e-01,   1.29199491e+00,

         2.25457336e-01,   1.30154339e+00,   6.07148640e-01,
          -5.95070426e-02,   6.21800593e-01,   1.57120710e+00,
          4.73504692e-01,   2.80268269e-01,  -9.44800133e-01,
          -1.38897006e+00,  -4.98320363e-01,   1.28625539e+00,
          7.92799822e-01,   1.25427742e+00,  -1.46892775e+00,
          -7.10541491e-01,  -3.65164634e-01,  -2.23467372e+00,

         1.13939645e+00,  -6.25826583e-01,   1.78198625e+00,
          -7.23262968e-02,   1.51546700e+00,  -3.74091024e-01,
          7.12866053e-02,   5.90149131e-01,   1.46031185e+00,
           8.76569968e-01,   2.87027567e-01,   7.89182456e-01,
         -1.34081620e+00,   7.98113003e-01,  -1.07803672e+00,
           4.92875900e-01,  -1.32762168e+00,  -1.31367497e+00,

         5.39240538e-01,  -6.74600551e-02,  -9.43384491e-01,
          -6.86552679e-01,   5.51378962e-01,   6.11858733e-01,
          8.87442300e-01,   5.52169782e-01,  -8.68143198e-01,
           1.89453070e-01,  -7.49185201e-01,  -1.44401744e+00,
          2.53292360e-01,  -2.48348866e-01,   5.52728868e-01,
           2.19671888e-03,   7.23640248e-01,   8.11576948e-01,

         1.77428801e+00,  -2.05153034e-01,  -4.92266266e-01,
           6.22191196e-01,  -9.17633549e-01,   3.53273308e-02,
          1.10851817e+00,   6.89986079e-01,  -3.29444595e-01,
           2.44250685e-02,   7.35834706e-01,   2.08749448e-01,
          7.04143250e-02,   9.92143864e-01,  -8.36567603e-01,
          -1.56015937e+00,  -9.91827434e-01,  -1.94415148e-01,

        -2.17932534e+00,   8.97398792e-01,   7.77070084e-01,
           1.37404702e+00,  -7.12934367e-01,   2.68901816e-01,
          9.16604920e-01,   6.87416231e-01,  -1.45759058e+00,
           6.99007330e-01,  -5.29149364e-01,   9.08301088e-02,
         -1.30014785e+00,  -5.42448743e-01,  -4.09521083e-01,
          -2.92314732e-02,  -1.10292940e+00,  -2.73228033e+00,

         9.50606039e-01,   4.58779516e-01,  -4.13384478e-01,
           8.98967895e-01,   2.14491377e+00,   9.32765864e-01,
         -5.44532811e-01,   2.08868672e+00,  -1.01337471e+00,
          -3.98631382e-01,   2.56432400e-01,  -6.31612556e-01,
          4.51229144e-01,   5.26110725e-01,   1.37100007e-02,
           1.09852716e+00,  -4.02645395e-01,  -8.32016376e-01,

        -8.27335442e-01,   1.71777065e-01,   3.16587615e-01,
          -2.68102708e-01,  -1.04386775e+00,  -1.11843196e+00,
          6.74132256e-01,   4.99546300e-03,   8.84726997e-01,
          -1.13801366e+00,   8.22529430e-01,  -9.06626773e-01,
          2.97478930e-02,  -1.09266151e+00,  -3.95794124e-02,
          -8.57433423e-01,   1.24854517e+00,  -4.80061940e-01;
    WordEmbedding we("{inputShape=[3];hidden=5;V=5;D=6}");   //inputShape=T
    *(we.params["W"])=W;

    t_cppl cache;
    t_cppl grads;
    t_cppl states;
    MatrixN y0=we.forward(x, &cache, &states);
    bool allOk=true;
    bool ret=matComp(y,y0,"WordEmbeddingBackward forward consistency check",eps);
    if (!ret) allOk=false;
    MatrixN dx0=we.backward(dchain, &cache, &states, &grads);
    //bool ret=matComp(dx,dx0,"WordEmbeddingBackward dx",eps);
    //if (!ret) allOk=false;
    ret=matComp(dW,*(grads["W"]),"WordEmbeddingBackward dW",eps);
    if (!ret) allOk=false;

    cppl_delete(&cache);
    cppl_delete(&grads);
    return allOk;
}

#endif
