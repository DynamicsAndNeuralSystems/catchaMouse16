#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "CO_TranslateShape.h"
#include "SY_SlidingWindow.h"
#include "stats.h"

double CO_TranslateShape_circle_35_pts(const double y[], const int size, const char whichOut[]) {
    
    // NAN check
    int i, j;
    for (i = 0; i < size; i++)
        if (isnan(y[i]))
            return NAN;
    
    double r = 3.5;
    int w = floor(r); // only consider a window radius w (these are the only
                    // points that could possibly be inside)
    int NN = size - 2*w; // number of admissible points

    double *np = (double*) malloc(NN * sizeof(double));
    double difwin_sum;
    int count;
    for (i = w+1; i <= size - w; i++) {
        difwin_sum = 0;
        count = 0;
        for (j = i-w; j <= i+w; j++) {
            difwin_sum = ((j - i)*(j - i)) + ((y[j-1] - y[i-1])*(y[j-1] - y[i-1]));
            if(difwin_sum <= r*r)
                count++;
        }
        // number of points enclosed in shape
        np[i-w-1] = (double)count;
    }
    
    double out;
    if (!strcmp(whichOut, "std"))
        out = stddev(np, NN);
    else if (!strcmp(whichOut, "statav4_m")) // Stationarity of the statistics in 4 segments of the time series
        out = SY_SlidingWindow(np, NN, "mean", "std", 4, 1);
    else
    {
        printf("Error in CO_TranslateShape: Unknown statistics or maybe not implemented!\n");
        out = NAN;
    }
    
    return out;
}

double CO_TranslateShape_circle_35_pts_statav4_m(const double y[], const int size) {
    return CO_TranslateShape_circle_35_pts(y, size, "statav4_m");
}

double CO_TranslateShape_circle_35_pts_std(const double y[], const int size) {
    return CO_TranslateShape_circle_35_pts(y, size, "std");
}
