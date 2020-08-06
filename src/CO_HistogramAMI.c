#include "CO_HistogramAMI.h"
#include "histcounts.h"
#include "helper_functions.h"
#include "stats.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

//#define tau 1 // before = 3
//#define numBins 10 // before = 2

double CO_HistogramAMI_even(const double y[], const int size, const int numBins, const int tau) {
    
    // NaN check
    for (int i = 0; i < size; i++)
        if (isnan(y[i]))
            return NAN;
    
    //int tau = 1;
    //int numBins = 10;
    
    double * y1 = malloc((size-tau) * sizeof(double));
    double * y2 = malloc((size-tau) * sizeof(double));
    
    for (int i = 0; i < size-tau; i++) {
        y1[i] = y[i];
        y2[i] = y[i+tau];
    }
    
    // set bin edges
    const double maxValue = max_(y, size);
    const double minValue = min_(y, size);
    
    double binStep = (maxValue - minValue + 0.2)/numBins; // problem
    //double binEdges[numBins+1] = {0};
	//double binEdges[10+1] = {0};
    double *binEdges = (double*) malloc((numBins+1) * sizeof(double));
    memset(binEdges, 0, (numBins+1)*sizeof(double));
    for (int i = 0; i < numBins+1; i++) {
        binEdges[i] = minValue + binStep*i - 0.1;
        //printf("binEdges[%i] = %1.3f\n", i, binEdges[i]);
    }
    
    
    // count histogram bin contents
    int * bins1;
    bins1 = histbinassign(y1, size-tau, binEdges, numBins+1);
    
    int * bins2;
    bins2 = histbinassign(y2, size-tau, binEdges, numBins+1);
    
    /*
    // debug
    for(int i = 0; i < size-tau; i++){
        printf("bins1[%i] = %i, bins2[%i] = %i\n", i, bins1[i], i, bins2[i]);
    }
    */
    
    // joint
    double *bins12 = (double*) malloc((size-tau) * sizeof(double));
    //double binEdges12[(numBins + 1) * (numBins + 1)] = {0};
	//double binEdges12[(10 + 1) * (10 + 1)] = {0};
    double *binEdges12 = (double*) malloc(((numBins+1) * (numBins+1)) * sizeof(double));
    memset(binEdges12, 0, ((numBins+1) * (numBins+1))*sizeof(double));
    
    for (int i = 0; i < size-tau; i++) {
        bins12[i] = (bins1[i]-1)*(numBins+1) + bins2[i];
        // printf("bins12[%i] = %1.3f\n", i, bins12[i]);
    }
    for (int i = 0; i < (numBins+1)*(numBins+1); i++) {
        binEdges12[i] = i+1;
        // printf("binEdges12[%i] = %1.3f\n", i, binEdges12[i]);
    }
    
    // fancy solution for joint histogram here
    int * jointHistLinear;
    jointHistLinear = histcount_edges(bins12, size-tau, binEdges12, (numBins + 1) * (numBins + 1));
    
    /*
    // debug
    for(int i = 0; i < (numBins+1)*(numBins+1); i++){
        printf("jointHistLinear[%i] = %i\n", i, jointHistLinear[i]);
    }
    */
    
    // transfer to 2D histogram (no last bin, as in original implementation)
    double pij[numBins][numBins];
    int sumBins = 0;
    for (int i = 0; i < numBins; i++) {
        for (int j = 0; j < numBins; j++) {
            pij[j][i] = jointHistLinear[i*(numBins+1)+j];
            
            // printf("pij[%i][%i]=%1.3f\n", i, j, pij[i][j]);
            
            sumBins += pij[j][i];
        }
    }
    
    // normalise
    for (int i = 0; i < numBins; i++)
        for (int j = 0; j < numBins; j++)
            pij[j][i] /= sumBins;

    // marginals
    //double pi[numBins] = {0};
	//double pi[10] = {0};
    double *pi = (double*) malloc(numBins * sizeof(double));
    memset(pi, 0, numBins*sizeof(double));
    //double pj[numBins] = {0};
	//double pj[10] = {0};
    double *pj = (double*) malloc(numBins * sizeof(double));
    memset(pj, 0, numBins*sizeof(double));
    for (int i = 0; i < numBins; i++) {
        for (int j = 0; j < numBins; j++) {
            pi[i] += pij[i][j];
            pj[j] += pij[i][j];
            //printf("pij[%i][%i]=%1.3f, pi[%i]=%1.3f, pj[%i]=%1.3f\n", i, j, pij[i][j], i, pi[i], j, pj[j]);
        }
    }
    
    /*
    // debug
    for(int i = 0; i < numBins; i++){
        printf("pi[%i]=%1.3f, pj[%i]=%1.3f\n", i, pi[i], i, pj[i]);
    }
    */
    // mutual information
    double ami = 0;
    for (int i = 0; i < numBins; i++) {
        for (int j = 0; j < numBins; j++) {
            if (pij[i][j] > 0) {
                //printf("I m in %lf\n", pij[i][j]);
                //printf("pij[%i][%i]=%1.3f, pi[%i]=%1.3f, pj[%i]=%1.3f, logarg=, %1.3f, log(...)=%1.3f\n",
                //       i, j, pij[i][j], i, pi[i], j, pj[j], pij[i][j]/(pi[i]*pj[j]), log(pij[i][j]/(pi[i]*pj[j])));
                ami += pij[i][j] * log(pij[i][j]/(pj[j]*pi[i]));
            }
        }
    }
    
    free(bins1);
    free(bins2);
    free(jointHistLinear);
    
    free(y1);
    free(y2);
    free(bins12);
    
    return ami;
}

double CO_HistogramAMI_even_10_1(const double y[], const int size) {
    return CO_HistogramAMI_even(y, size, 10, 1);
}

double CO_HistogramAMI_even_10_3(const double y[], const int size) {
    return CO_HistogramAMI_even(y, size, 10, 3);
}

double CO_HistogramAMI_even_2_3(const double y[], const int size) {
    return CO_HistogramAMI_even(y, size, 2, 3);
}
