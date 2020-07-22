#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "PH_Walker.h"
#include "stats.h"
#include "helper_functions.h"

double PH_Walker_momentum_5_w_momentumzcross(const double y[], const int size) {
    
    // check NAN
    int i, j;
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
    double w_propzcross;
    for (i = 1; i < size; i++)
    {
        if ((w[i-1] * w[i]) < 0)
            w_propzcross++;
    }
    w_propzcross /= (size - 1);
    free(w);

    return w_propzcross;
}
