#if __cplusplus
#   include <complex>
typedef std::complex< double > cplx;
#else
#   include <complex.h>
#if defined(__GNUC__) || defined(__GNUG__)
typedef double complex cplx;
#elif defined(_MSC_VER)
typedef _Dcomplex cplx;
#endif
#endif

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "stats.h"
#include "fft.h"

#include "helper_functions.h"

#ifndef CMPLX
#define CMPLX(x, y) ((cplx)((double)(x) + _Imaginary_I * (double)(y)))
#endif
#define pow2(x) (1 << x)

int nextpow2(int n) {
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n++;
    return n;
}

/*
static void apply_conj(cplx a[], int size, int normalize)
{   
    switch(normalize) {
        case(1):
            for (int i = 0; i < size; i++) {
                a[i] = conj(a[i]) / size;
            }
            break;
        default:
            for (int i = 0; i < size; i++) {
                a[i] = conj(a[i]);
            }
            break;
    }
}
 */

void dot_multiply(cplx a[], cplx b[], int size) {
    for (int i = 0; i < size; i++)
        a[i] = _Cmulcc(a[i], conj(b[i]));
}

double * CO_AutoCorr(const double y[], const int size, const int tau[], const int tau_size) {
    double m, nFFT;
    m = mean(y, size);
    nFFT = nextpow2(size) << 1;

    cplx * F = malloc(nFFT * sizeof *F);
    cplx * tw = malloc(nFFT * sizeof *tw);
    for (int i = 0; i < size; i++) {
        
        #if defined(__GNUC__) || defined(__GNUG__)
                F[i] = CMPLX(y[i] - m, 0.0);
        #elif defined(_MSC_VER)
                cplx tmp = { y[i] - m, 0.0 };
                F[i] = tmp;
        #endif
        
    }
    for (int i = size; i < nFFT; i++) {
        #if defined(__GNUC__) || defined(__GNUG__)
                F[i] = CMPLX(0.0, 0.0);
        #elif defined(_MSC_VER)
                cplx tmp = { 0.0, 0.0 };
                F[i] = tmp; // CMPLX(0.0, 0.0);
        #endif
        
    }
    // size = nFFT;

    twiddles(tw, nFFT);
    fft(F, nFFT, tw);
    dot_multiply(F, F, nFFT);
    fft(F, nFFT, tw);
    cplx divisor = F[0];
    for (int i = 0; i < nFFT; i++) {
        //F[i] = F[i] / divisor;
        F[i] = _Cdivcc(F[i], divisor);
    }
    
    double * out = malloc(tau_size * sizeof(out));
    for (int i = 0; i < tau_size; i++) {
        out[i] = creal(F[tau[i]]);
    }
    free(F);
    free(tw);
    return out;
}

double * co_autocorrs(const double y[], const int size) {
    double m, nFFT;
    m = mean(y, size);
    nFFT = nextpow2(size) << 1;
    
    cplx * F = malloc(nFFT * 2 * sizeof *F);
    cplx * tw = malloc(nFFT * 2 * sizeof *tw);
    for (int i = 0; i < size; i++) {
        
        #if defined(__GNUC__) || defined(__GNUG__)
                F[i] = CMPLX(y[i] - m, 0.0);
        #elif defined(_MSC_VER)
                cplx tmp = { y[i] - m, 0.0 };
                F[i] = tmp;
        #endif
    }
    for (int i = size; i < nFFT; i++) {
        
        #if defined(__GNUC__) || defined(__GNUG__)
            F[i] = CMPLX(0.0, 0.0);
        #elif defined(_MSC_VER)
            cplx tmp = { 0.0, 0.0 };
            F[i] = tmp;
        #endif
    }
    //size = nFFT;
    
    twiddles(tw, nFFT);
    fft(F, nFFT, tw);
    dot_multiply(F, F, nFFT);
    fft(F, nFFT, tw);
    cplx divisor = F[0];
    for (int i = 0; i < nFFT; i++)
        F[i] = _Cdivcc(F[i], divisor); // F[i] / divisor;
    
    double * out = malloc(nFFT * 2 * sizeof(out));
    for (int i = 0; i < nFFT; i++)
        out[i] = creal(F[i]);

    free(F);
    free(tw);
    return out;
}

int co_firstzero(const double y[], const int size, const int maxtau) {
    
    //double * autocorrs = malloc(size * sizeof * autocorrs);
    //autocorrs = co_autocorrs(y, size);
    
    double * autocorrs = co_autocorrs(y, size);
    
    int zerocrossind = 0;
    while(autocorrs[zerocrossind] > 0 && zerocrossind < maxtau)
        zerocrossind += 1;
    
    free(autocorrs);
    return zerocrossind;
    
}
