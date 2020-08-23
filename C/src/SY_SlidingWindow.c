#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "SY_SlidingWindow.h"
#include "stats.h"

double SY_SlidingWindow(const double y[], const int size, const char windowStat[], const char acrossWinStat[], int numSeg, int incMove) {
    
    // NAN check
    int i, j;
    for (i = 0; i < size; i++)
        if (isnan(y[i]))
            return NAN;

    int winlen = floor(size/numSeg);
    int inc = floor(winlen/incMove);
    if (inc == 0)
        inc++;
    
    int numSteps = floor((size - winlen) / inc) + 1;
    double *qs = (double*) malloc(numSteps * sizeof(double));
    
    if (!strcmp(windowStat, "mean")) {
        for (i = 1; i <= numSteps; i++) {
            qs[i-1] = 0;
            for (j = (i-1)*inc + 1; j <= (i-1)*inc + winlen; j++)
                qs[i-1] += y[j-1];
            qs[i-1] /= winlen;
        }
    }
    else if (!strcmp(windowStat, "std")) {
        double m;
        for (i = 1; i <= numSteps; i++) {
            m = 0;
            for (j = (i-1)*inc + 1; j <= (i-1)*inc + winlen; j++)
                m += y[j - 1];
            m /= winlen;
            
            qs[i-1] = 0;
            for (j = (i-1)*inc + 1; j <= (i-1)*inc + winlen; j++)
                qs[i-1] += pow(y[j-1] - m, 2);
            qs[i-1] = sqrt(qs[i-1] / (winlen-1));
        }
    }
    else {
        printf("Error in SY_SlidingWindow: Unknown or Other Statistics!\n");
        return NAN;
    }
    
    // NAN check
    for (i = 0; i < numSteps; i++)
        if (isnan(qs[i]))
            return NAN;

    double out;
    if (!strcmp(acrossWinStat, "std")) {
        out = stddev(qs, numSteps) / stddev(y, size);
    }
    else {
        printf("Error in SY_SlidingWindow: Unknown or Other Statistics!\n");
        return NAN;
    }

    free(qs);
    return out;
}
