#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <gsl/gsl_randist.h>
#include <sys/time.h>

#include "CO_AddNoise.h"
#include "helper_functions.h"
#include "histcounts.h"
#include "stats.h"
#include "CO_HistogramAMI.h"

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
}

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
}*/

double CO_AddNoise_1_even_10_ami_at_10(const double y[], const int size) {

    // check NAN
    int i, j;
    for (i = 0; i < size; i++)
        if (isnan(y[i]))
            return NAN;

    double *noise = (double*) malloc(size * sizeof(double));
    double *yn = (double*) malloc(size * sizeof(double));

    gsl_rng * rr;
    double mu = 1.0;

    gsl_rng_env_setup();
    rr = gsl_rng_alloc (gsl_rng_mt19937);
    //struct timeval tv; // Seed generation based on time
    //gettimeofday(&tv,0);
    //unsigned long seed = iter;//tv.tv_sec + tv.tv_usec;
    long seed = 0;
    gsl_rng_set(rr, seed);
    
    // twister random generator
    //printf("noise is:\n");
    for (i = 0; i < size; i++)
        noise[i] = gsl_ran_gaussian_ziggurat(rr, mu);

    int numRepeats = 50;
    double *noiseRange = (double*) malloc(numRepeats * sizeof(double));
    linspace(0, 3, numRepeats, noiseRange);
    

    double *amis = (double*) malloc(numRepeats * sizeof(double));
    memset(amis, 0, numRepeats * sizeof(double));

    for (i = 0; i < numRepeats; i++) {
        for (j = 0; j < size; j++)
            yn[j] = y[j] + (noiseRange[i] * noise[j]);

        amis[i] = CO_HistogramAMI_even_10_1(yn, size);
        if (isnan(amis[i])) {
            printf("Error computing AMI: Time series too short (?)");
            return NAN;
        }
    }
    
    double out = 0;
    for (i = 0; i < numRepeats; i++) {
        if (noiseRange[i] >= 1) { // noiseLevel = 1
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
