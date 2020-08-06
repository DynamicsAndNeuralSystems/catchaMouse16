#include <stdio.h>
#include <math.h>
#include "stats.h"

#include "CO_NonlinearAutocorr.h"

double CO_NonlinearAutocorr(const double y[], const int size, const int taus[]) {

    int tmax = taus[2];
    
    double *nlac = (double*) malloc((size) * sizeof(double));

    int i, j;
    for (i = tmax; i < size; i++)
        nlac[ i - tmax ] = y[i];

    int size_nlac = size - tmax;
    
    for (i = 0; i < 3; i++) {
        int start = tmax - taus[i];
        for (j = start; j < size - taus[i]; j++)
            nlac[j-start] = nlac[j-start]*y[j];
    }
    double out = mean(nlac, size_nlac);
    free(nlac);

    return out;
}

double AC_nl_035(const double y[], const int size) {
    
    int taus[3] = {0,3,5};

    return CO_NonlinearAutocorr(y, size, taus);
}

double AC_nl_036(const double y[], const int size) {
    
    int taus[3] = {0,3,6};

    return CO_NonlinearAutocorr(y, size, taus);
}

double AC_nl_112(const double y[], const int size) {

    int taus[3] = {1,1,2};
    
    return CO_NonlinearAutocorr(y, size, taus);
}
