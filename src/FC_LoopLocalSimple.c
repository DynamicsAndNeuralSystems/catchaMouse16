#include "FC_LoopLocalSimple.h"
//#include "CO_AutoCorr.h"
//#include "SY_SlidingWindow.h"
#include "stats.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

double FC_LocalSimple(const double y[], const int size, int trainLength) {

    // Method:- mean
    int i, j;
    int lp = trainLength;

    // range over which to evaluate
    int evalr_len = size - lp; //lp+1:N
    if (evalr_len == 0) {
        printf("FC_LocalSimple: Time Series too short for forecasting\n");
        return NAN;
    }
    int *evalr = (int*) malloc(evalr_len * sizeof(int));
    for (i = 0; i < evalr_len; i++)
        evalr[i] = i + lp + 1;

    double *res = (double*) malloc(evalr_len * sizeof(double));
    memset(res, 0, evalr_len * sizeof(double));
    for (i = 0; i < evalr_len; i++) {
        for (j = evalr[i] - lp - 1; j < evalr[i] - 1; j++)
            res[i] += y[j];
        res[i] = (res[i]/lp) - y[evalr[i] - 1];
    }

    double out = stddev(res, evalr_len);
    /*out[1] = SY_SlidingWindow(res, evalr_len, "std", "std", 5, 1); // sws
    out[2] = SY_SlidingWindow(res, evalr_len, "mean", "std", 5, 1); // swm
    int tau[1] = {1};
    out[3] = *CO_AutoCorr(res, evalr_len, tau, 1); // ac1
    tau[0] = 2;
    out[4] = *CO_AutoCorr(res, evalr_len, tau, 1); // ac2*/

    free(evalr);
    free(res);
    return out;
}

double FC_LoopLocalSimple_mean_stderr_chn(const double y[], const int size) {

    // Check NAN
    int i;
    for (i = 0; i < size; i++)
        if (isnan(y[i]))
            return NAN;
    
    int trainLengthRange = 10;
    double *stats_st = (double*) malloc(trainLengthRange * sizeof(double)); // 10 x 5 matrix
    double mi = INT_MAX, ma = -INT_MAX;
    for (i = 0; i < trainLengthRange; i++) {
        stats_st[i] = FC_LocalSimple(y, size, i+1);
        if (mi > stats_st[i])   mi = stats_st[i];
        if (ma < stats_st[i])   ma = stats_st[i];
    }
    double range = ma - mi;

    double *st_diff = (double*) malloc((trainLengthRange - 1) * sizeof(double));
    diff(stats_st, trainLengthRange, st_diff);
    double stderr_chn = mean(st_diff, trainLengthRange - 1)/ range;
    
    free(stats_st);
    free(st_diff);

    return stderr_chn;
}
