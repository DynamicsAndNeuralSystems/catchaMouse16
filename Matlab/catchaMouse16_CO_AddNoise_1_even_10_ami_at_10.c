#include <math.h>
#include "mex.h"

#include "M_wrapper.h"

#include "../src/CO_AddNoise.h"
#include "../src/helper_functions.h"
#include "../src/histcounts.h"
#include "../src/stats.h"
#include "../src/CO_HistogramAMI.h"

void mexFunction( int nlhs, mxArray *plhs[], 
      int nrhs, const mxArray*prhs[] )
     
{ 
    
    // check inputs and call feature C-function
M_wrapper_double( nlhs, plhs, nrhs, prhs, &CO_AddNoise_1_even_10_ami_at_10, 1);
    
    return;
    
}
