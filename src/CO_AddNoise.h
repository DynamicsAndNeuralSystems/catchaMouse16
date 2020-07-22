#ifndef CO_ADDNOISE_H
#define CO_ADDNOISE_H

#define PI 3.141592654
#include "mtwister.h"
extern double CO_HistogramAMI_even_10_1(const double y[], const int size);
extern double gaussrand(MTRand* rand); 
extern double CO_AddNoise_1_even_10_ami_at_10(const double y[], const int size);

#endif
