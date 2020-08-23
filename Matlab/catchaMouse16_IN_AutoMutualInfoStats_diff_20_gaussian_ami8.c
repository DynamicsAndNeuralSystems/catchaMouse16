#include <math.h>
#include "mex.h"

#include "M_wrapper.h"

#include "../C/src/CO_AutoCorr.h"
#include "../C/src/stats.h"
#include "../C/src/IN_AutoMutualInfoStats.h"

void mexFunction( int nlhs, mxArray *plhs[], 
      int nrhs, const mxArray*prhs[] )
     
{ 
    
    // check inputs and call feature C-function
M_wrapper_double( nlhs, plhs, nrhs, prhs, &IN_AutoMutualInfoStats_diff_20_gaussian_ami8, 1);
    
    return;
    
}
