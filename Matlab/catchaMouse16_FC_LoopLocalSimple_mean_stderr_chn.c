#include <math.h>
#include "mex.h"

#include "M_wrapper.h"

#include "../C/src/FC_LoopLocalSimple.h"
#include "../C/src/stats.h"

void mexFunction( int nlhs, mxArray *plhs[], 
      int nrhs, const mxArray*prhs[] )
     
{ 
    
    // check inputs and call feature C-function
M_wrapper_double( nlhs, plhs, nrhs, prhs, &FC_LoopLocalSimple_mean_stderr_chn, 1);
    
    return;
    
}
