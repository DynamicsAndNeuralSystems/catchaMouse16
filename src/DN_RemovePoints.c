#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "stats.h"
#include "CO_AutoCorr.h"
#include "DN_RemovePoints.h"
#include "helper_functions.h"


// Comparator for struct array type, to sort in descending order
int compare_array(const void * a, const void * b) {
    struct array *na = (struct array *)a;
    struct array *nb = (struct array *)b;
    int res;
    if(na->data < nb->data)
        res = 1;
    if(na->data == nb->data)
        res = 0;
    if(na->data > nb->data)
        res = -1;
    return res;
}

// Comparator for integer type, to sort in ascending order
int compare_int(const void * a, const void * b) {
    return ( *(const int*)a - *(const int*)b ); 
}

// Stores absolute value of y elements in struct array
void abs_array(const double y[], int size, struct array *abs_y) {
    int i;
    for(i = 0; i < size; i++) {
        abs_y[i].data = fabs(y[i]);
        abs_y[i].ind = i;
    }
}

// Computes autocorrelation of the input sequence, y, up to a maximum time lag
double* SUB_acf(const double y[], const int size, double acf[], int lag) {
    int i;
    for (i = 1; i <= lag; i++)
        acf[i-1] = *CO_AutoCorr(y, size, &i, 1);
    return acf;
}

double DN_RemovePoints_absclose_05_ac2rat(const double y[], const int size) {
    
    // NaN check
    int i;
    for (i = 0; i < size; i++)
        if (isnan(y[i]))
            return NAN;

    // Sort the array with absolute values
    struct array *abs_y = (struct array*) malloc(size * sizeof(struct array));
    abs_array(y, size, abs_y);
    
    qsort(abs_y, size, sizeof(struct array), compare_array);

    // get sorted indices and keep first 50% of array
    int keep_size = round(size*0.5);
    int *sorted_ind = (int *) malloc(keep_size * sizeof(int));
    
    for (i = 0; i < keep_size; i++)
        sorted_ind[i] = abs_y[i].ind;  

    // sort the indices
    qsort(sorted_ind, keep_size, sizeof(int), compare_int);

    double *yTransform = (double *) malloc(keep_size * sizeof(double));
    for (i = 0; i < keep_size; i++)
        yTransform[i] = y[sorted_ind[i]];
    
    int tau[1] = {2};
    double acf_y = *CO_AutoCorr(y, size, tau, 1);
    double acf_yTransform = *CO_AutoCorr(yTransform, keep_size, tau, 1);
    double ac2rat = acf_yTransform/acf_y;

    free(abs_y);
    free(sorted_ind);
    free(yTransform);    

    return ac2rat;
}
