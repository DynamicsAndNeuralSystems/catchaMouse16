#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "CO_TranslateShape.h"
#include "SY_SlidingWindow.h"

double CO_TranslateShape_circle_35_pts_statav4_m(const double y[], const int size) {
    
    // NAN check
    int i, j;
    for(i = 0; i < size; i++)
        if(isnan(y[i]))
            return NAN;
    
    double r = 3.5;
    int w = floor(r); // only consider a window radius w (these are the only
                    // points that could possibly be inside)
    int NN = size - 2*w; // number of admissible points

    double *np = (double*) malloc(NN * sizeof(double));
    double difwin_sum;
    int count;
    for(i = w+1; i <= size - w; i++)
    {
        difwin_sum = 0;
        count = 0;
        for(j = i-w; j <= i+w; j++)
        {
            difwin_sum = ((j - i)*(j - i)) + ((y[j-1] - y[i-1])*(y[j-1] - y[i-1]));
            if(difwin_sum <= r*r)
                count++;
        }
        // number of points enclosed in shape
        np[i-w-1] = (double)count;
    }
    
    // Stationarity of the statistics in 4 segments of the time series
    double out = SY_SlidingWindow(np, NN);
    return out;
}
