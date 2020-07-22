#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "SY_SlidingWindow.h"
#include "stats.h"

double SY_SlidingWindow(const double y[], const int size) {
    
    // NAN check
    int i, j;
    for(i = 0; i < size; i++)
        if(isnan(y[i]))
            return NAN;

    // numSeg = 4
    int winlen = floor(size/4);
    int inc = winlen;
    int numSteps = floor((size - winlen) / inc) + 1;
    double *qs = (double*) malloc(numSteps * sizeof(double));
    
    for(i = 1; i <= numSteps; i++)
    {
        qs[i-1] = 0;
        for(j = (i-1)*inc + 1; j <= (i-1)*inc + winlen; j++)
            qs[i-1] += y[j-1];
        qs[i-1] /= winlen;
    }

    // NAN check
    for(i = 0; i < numSteps; i++)
        if(isnan(qs[i]))
            return NAN;

    double out = stddev(qs, numSteps) / stddev(y, size);

    free(qs);
    return out;
}
