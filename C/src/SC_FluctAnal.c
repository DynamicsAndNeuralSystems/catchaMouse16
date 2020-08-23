#include <math.h>
#include <string.h>
#include <time.h>
#include <float.h>
#include <stdio.h>
#include "stats.h"
#include "CO_AutoCorr.h"
#include "SC_FluctAnal.h"

#include <gsl/gsl_multifit.h>
#include <gsl/gsl_randist.h>

double dofit(const gsl_multifit_robust_type *T, const gsl_matrix *X, const gsl_vector *y,
      gsl_vector *c, gsl_matrix *cov, int n)
{    
    int s;
    gsl_set_error_handler_off();

    gsl_multifit_robust_workspace * work = gsl_multifit_robust_alloc(T, X->size1, X->size2);

    s = gsl_multifit_robust(X, y, c, cov, work);
    
    //gsl_multifit_robust_stats stat = gsl_multifit_robust_statistics(work);
    return sqrt(gsl_matrix_get(cov, 1, 1)); // standard error of mean
}

// --- taustep = 50, k = 2 ---
double SC_FluctAnal_2_dfa_50_2_logi_r2_se2(const double y[], const int size) {
    
    // NaN check
    for (int i = 0; i < size; i++)
        if (isnan(y[i]))
            return NAN;
    
    //int lag = 2;

    // generate log spaced tau vector
    double linLow = log(5);
    double linHigh = log(size/2);
    
    int nTauSteps = 50;
    double tauStep = (linHigh - linLow) / (nTauSteps-1);
    
    int tau[50];
    for (int i = 0; i < nTauSteps; i++)
        tau[i] = round(exp(linLow + i*tauStep));
    
    // check for uniqueness, use ascending order
    int nTau = nTauSteps;
    for (int i = 0; i < nTauSteps-1; i++) {
        while (tau[i] == tau[i+1] && i < nTau-1) {
            for (int j = i+1; j < nTauSteps-1; j++)
                tau[j] = tau[j+1];
            // lost one
            nTau -= 1;
        }
    }
    
    // fewer than 12 points -> leave.
    if (nTau < 12) {
        return 0;
    }
    
    int sizeCS = size;
    double * yCS = malloc(sizeCS * sizeof(double));
    
    // transform input vector to cumsum
    yCS[0] = y[0];
    for (int i = 0; i < sizeCS-1; i++)
        yCS[i+1] = yCS[i] + y[(i+1)];
    
    //for each value of tau, cut signal into snippets of length tau, detrend and
    
    // first generate a support for regression (detrending)
    double * xReg = malloc(tau[nTau-1] * sizeof * xReg);
    for (int i = 0; i < tau[nTau-1]; i++)
        xReg[i] = i+1;
    
    // iterate over taus, cut signal, detrend and save amplitude of remaining signal
    double * F = malloc(nTau * sizeof * F);
    for (int i = 0; i < nTau; i++) {
        int nBuffer = sizeCS/tau[i];
        double * buffer = malloc(tau[i] * sizeof * buffer);
        //double m = 0.0, b = 0.0;
        double *coeff = (double*) malloc(3 * sizeof(double));
        //printf("tau[%i]=%i\n", i, tau[i]);
        
        F[i] = 0;
        for (int j = 0; j < nBuffer; j++) {
            /* Polynomial linear regression of order 2 */

            //linreg(tau[i], xReg, yCS+j*tau[i], &m, &b); // quad_linreg() -- 3 coefficients
            polynomialfit(tau[i], 2 + 1, xReg, yCS+j*tau[i], coeff);
            //printf("%lf, %lf, %lf\n", coeff[0], coeff[1], coeff[2]);
            
            for (int k = 0; k < tau[i]; k++) {
                buffer[k] = yCS[j*tau[i]+k] - ((coeff[2] * pow(k+1, 2)) + (coeff[1] * (k+1)) + coeff[0]);
                //printf("buffer[%i]=%1.3f\n", k, buffer[k]);
            }
            for(int k = 0; k < tau[i]; k++)
                F[i] += buffer[k]*buffer[k];
        }
        
        //if (strcmp(how, "dfa") == 0)
        F[i] = sqrt(F[i] / (nBuffer * tau[i]));        
        free(buffer);
    }
    
    double * logtt = malloc(nTau * sizeof * logtt);
    double * logFF = malloc(nTau * sizeof * logFF);
    int ntt = nTau;
    
    for (int i = 0; i < nTau; i++) {
        logtt[i] = log(tau[i]);
        logFF[i] = log(F[i]);
    }
    
    int minPoints = 6;
    int nsserr = (ntt - 2*minPoints + 1);
    double * sserr = malloc(nsserr * sizeof * sserr);
    double * buffer = malloc((ntt - minPoints + 1) * sizeof * buffer);
    for (int i = minPoints; i < ntt - minPoints + 1; i++) {
        // this could be done with less variables of course
        double m1 = 0.0, b1 = 0.0;
        double m2 = 0.0, b2 = 0.0;
        
        sserr[i - minPoints] = 0.0; // i
        
        linreg(i, logtt, logFF, &m1, &b1);
        linreg(ntt-i+1, logtt+i-1, logFF+i-1, &m2, &b2); 
        
        for (int j = 0; j < i; j ++)
            buffer[j] = logtt[j] * m1 + b1 - logFF[j];
        
        sserr[i - minPoints] += norm_(buffer, i); // i
        
        for (int j = 0; j < ntt-i+1; j++)
            buffer[j] = logtt[j+i-1] * m2 + b2 - logFF[j+i-1];
        
        sserr[i - minPoints] += norm_(buffer, ntt-i+1); // i        
    }

    double firstMinInd = 0.0;
    double minimum = min_(sserr, nsserr);

    for (int i = 0; i < nsserr; i++) {
        if (sserr[i] == minimum) {
            firstMinInd = i + minPoints - 1;
            break;
        }
    }
    int r2_len = ntt - firstMinInd;
    int *r2 = (int*) malloc((r2_len)*sizeof(int));
    
    gsl_vector *r2_logtt, *r2_logFF;
    r2_logtt = gsl_vector_alloc(r2_len);
    r2_logFF = gsl_vector_alloc(r2_len);
    for (int i = 0; i < r2_len; i++) {
        r2[i] = firstMinInd + i;
        gsl_vector_set(r2_logtt, i, logtt[r2[i]]);
        gsl_vector_set(r2_logFF, i, logFF[r2[i]]);
    }
    /* Robust fitting starts here (uses bi-squared objective function) */

    const size_t p = 2; // linear fit -- two coefficients 
    gsl_matrix *X, *cov;
    gsl_vector *c;
    c = gsl_vector_alloc(p);
    X = gsl_matrix_alloc(r2_len, p);
    cov = gsl_matrix_alloc(p, p);
    // construct design matrix X for linear fit
    for (int i = 0; i < r2_len; ++i) {
        double xi = gsl_vector_get(r2_logtt, i);

        gsl_matrix_set(X, i, 0, 1);
        gsl_matrix_set(X, i, 1, xi);
    }
    double out = dofit(gsl_multifit_robust_bisquare, X, r2_logFF, c, cov, r2_len);

    gsl_matrix_free(X);
    gsl_vector_free(c);
    gsl_matrix_free(cov);
    gsl_vector_free(r2_logtt);
    gsl_vector_free(r2_logFF);
    free(r2);
    free(yCS);
    free(xReg);
    free(F);
    free(logtt);
    free(logFF);
    free(sserr);
    free(buffer);
    return out;
}
