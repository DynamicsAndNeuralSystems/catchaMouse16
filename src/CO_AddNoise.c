#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "CO_AddNoise.h"
#include "helper_functions.h"
#include "histcounts.h"
#include "stats.h"
#include "mtwister.h"

#define tau 1 // before = 3
#define numBins 10 // before = 2

double CO_HistogramAMI_even_10_1(const double y[], const int size)
{
    
    // NaN check
    for(int i = 0; i < size; i++)
    {
        if(isnan(y[i]))
        {
            return NAN;
        }
    }
    
    //const int tau = 2;
    //const int numBins = 5;
    
    double * y1 = malloc((size-tau) * sizeof(double));
    double * y2 = malloc((size-tau) * sizeof(double));
    
    for(int i = 0; i < size-tau; i++){
        y1[i] = y[i];
        y2[i] = y[i+tau];
    }
    
    // set bin edges
    const double maxValue = max_(y, size);
    const double minValue = min_(y, size);
    
    double binStep = (maxValue - minValue + 0.8)/10;
    //double binEdges[numBins+1] = {0};
	double binEdges[10+1] = {0};
    for(int i = 0; i < numBins+1; i++){
        binEdges[i] = minValue + binStep*i - 0.1;
        // printf("binEdges[%i] = %1.3f\n", i, binEdges[i]);
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
    double * bins12 = malloc((size-tau) * sizeof(double));
    //double binEdges12[(numBins + 1) * (numBins + 1)] = {0};
	double binEdges12[(10 + 1) * (10 + 1)] = {0};    

    for(int i = 0; i < size-tau; i++){
        bins12[i] = (bins1[i]-1)*(numBins+1) + bins2[i];
        // printf("bins12[%i] = %1.3f\n", i, bins12[i]);
    }
    
    for(int i = 0; i < (numBins+1)*(numBins+1); i++){
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
    for(int i = 0; i < numBins; i++){
        for(int j = 0; j < numBins; j++){
            pij[j][i] = jointHistLinear[i*(numBins+1)+j];
            
            // printf("pij[%i][%i]=%1.3f\n", i, j, pij[i][j]);
            
            sumBins += pij[j][i];
        }
    }
    
    // normalise
    for(int i = 0; i < numBins; i++){
        for(int j = 0; j < numBins; j++){
            pij[j][i] /= sumBins;
        }
    }

    // marginals
    //double pi[numBins] = {0};
	double pi[10] = {0};
    //double pj[numBins] = {0};
	double pj[10] = {0};
    for(int i = 0; i < numBins; i++){
        for(int j = 0; j < numBins; j++){
            pi[i] += pij[i][j];
            pj[j] += pij[i][j];
            // printf("pij[%i][%i]=%1.3f, pi[%i]=%1.3f, pj[%i]=%1.3f\n", i, j, pij[i][j], i, pi[i], j, pj[j]);
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
    for(int i = 0; i < numBins; i++){
        for(int j = 0; j < numBins; j++){
            if(pij[i][j] > 0){
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

/*double gaussrand(MTRand* rand)
{
	static double V1, V2, S;
	static int phase = 0;
	double X;

	if(phase == 0) {
		do {
			double U1 = (double)genRand(rand) / RAND_MAX;
			double U2 = (double)genRand(rand) / RAND_MAX;

			V1 = 2 * U1 - 1;
			V2 = 2 * U2 - 1;
			S = V1 * V1 + V2 * V2;
			} while(S >= 1 || S == 0);

		X = V1 * sqrt(-2 * log(S) / S);
	} else
		X = V2 * sqrt(-2 * log(S) / S);

	phase = 1 - phase;

	return X;
}*/

double gaussrand(MTRand* rand)
{
	static double U, V;
	static int phase = 0;
	double Z;

	if(phase == 0) {
		U = (genRand(rand) + 1.) / (RAND_MAX + 2.);
		V = genRand(rand) / (RAND_MAX + 1.);
		Z = sqrt(-2 * log(U)) * sin(2 * PI * V);
	} else
		Z = sqrt(-2 * log(U)) * cos(2 * PI * V);

	phase = 1 - phase;

	return Z;
}

double CO_AddNoise_1_even_10_ami_at_10(const double y[], const int size) {

    // check NAN
    int i, j;
    for (i = 0; i < size; i++)
        if (isnan(y[i]))
            return NAN;

    double *noise = (double*) malloc(size * sizeof(double));
    double *yn = (double*) malloc(size * sizeof(double));

    // twister random generator
    MTRand r = seedRand(0);
    for (i = 0; i < size; i++)
        noise[i] = gaussrand(&r);

    int numRepeats = 50;
    double *noiseRange = (double*) malloc(numRepeats * sizeof(double));
    linspace(0, 3, numRepeats, noiseRange);
    

    double *amis = (double*) malloc(numRepeats * sizeof(double));
    memset(amis, 0, numRepeats * sizeof(double));

    for (i = 0; i < numRepeats; i++)
    {
        for (j = 0; j < size; j++)
            yn[j] = y[j] + (noiseRange[i] * noise[j]);

        amis[i] = CO_HistogramAMI_even_10_1(yn, size);
        if (isnan(amis[i]))
        {
            printf("Error computing AMI: Time series too short (?)");
            return NAN;
        }
    }
    
    double out;
    for (i = 0; i < numRepeats; i++)
    {
        if(noiseRange[i] >= 1)// noiseLevel = 1
        {
            out = amis[i];
            break;
        }
    }

    free(noise);
    free(noiseRange);
    free(amis);
    free(yn);

    return out;
}
