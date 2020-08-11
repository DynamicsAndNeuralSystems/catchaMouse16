#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "PH_Walker.h"
#include "stats.h"
#include "helper_functions.h"

double PH_Walker_momentum_5_w_momentumzcross(const double y[], const int size) {
    
    // check NAN
    int i;
    for (i = 0; i < size; i++)
        if (isnan(y[i]))
            return NAN;
    
    double *w = (double*) malloc(size * sizeof(double));
    double m = 5;

    w[0] = y[0];
    w[1] = y[1];

    double w_inert;
    for (i = 2; i < size; i++)
    {
        w_inert = w[i-1] + (w[i-1] - w[i-2]);
        w[i] = w_inert + (y[i] - w_inert)/m;
    }
    double w_propzcross = 0;
    for (i = 1; i < size; i++)
    {
        if ((w[i-1] * w[i]) < 0)
            w_propzcross++;
    }
    w_propzcross /= (size - 1);
    free(w);

    return w_propzcross;
}

double PH_Walker_biasprop_05_01_sw_meanabsdiff(const double y[], const int size) {
    
    // check NAN
    int i;
    for (i = 0; i < size; i++)
        if (isnan(y[i]))
            return NAN;
    
    // param = {0.5, 0.1}
    double *w = (double*) malloc(size * sizeof(double));
    double pup = 0.5;
    double pdown = 0.1;

    w[0] = 0;
    for (i = 1; i < size; i++) {
        if (y[i] > y[i - 1])
            w[i] = w[i - 1] + pup * (y[i - 1] - w[i - 1]);
        else
            w[i] = w[i - 1] + pdown * (y[i - 1] - w[i - 1]);
    }

    double sw_meanabsdiff = 0;
    for (i = 0; i < size; i++)
        sw_meanabsdiff += fabs(y[i] - w[i]);
    sw_meanabsdiff /= (size);
    
    free(w);

    return sw_meanabsdiff;
}
