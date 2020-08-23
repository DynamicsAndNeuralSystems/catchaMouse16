#include <math.h>
#include "mex.h"

#include "M_wrapper.h"

#include "../C/src/SY_DriftingMean.h"

void mexFunction( int nlhs, mxArray *plhs[], 
      int nrhs, const mxArray*prhs[] )
     
{ 
    
    // check inputs and call feature C-function
M_wrapper_double( nlhs, plhs, nrhs, prhs, &SY_DriftingMean50_min, 1);
    
    return;
    
}
