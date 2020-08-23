#include "SY_DriftingMean.h"

double SY_DriftingMean50_min(const double y[], const int size) { // let size = 200

    // check NAN
    int i, j;
    for (i = 0; i < size; i++)
        if (isnan(y[i]))
            return NAN;
        
    int l = 50; // Divide the time series into 50-sample segments
    int numFits = floor(size/l); // = 4

    double (*z)[l] = malloc( sizeof( double[numFits][l] ) );

    for (i = 0 ; i < numFits ; i++)
        for (j = 0; j < l ; j++)
            z[i][j] = y[i*l+j]; // i*l to (i+1)*l

    double *zm = malloc(numFits * sizeof(double));
    double *zv = malloc(numFits * sizeof(double));

    for (i = 0 ; i < numFits; i++) {
        zm[i] = mean(z[i], l);
        zv[i] = var(z[i], l);
    }

    double meanvar = mean(zv, numFits);
    double minmean = min_(zm, numFits);

    double out= minmean/meanvar;
    free(zm);
    free(zv);
    free(z);
    
    return out;   
}
