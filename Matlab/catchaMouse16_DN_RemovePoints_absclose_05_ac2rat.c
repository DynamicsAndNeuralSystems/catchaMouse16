#include <math.h>
#include "mex.h"

#include "M_wrapper.h"

#include "../src/stats.h"
#include "../src/CO_AutoCorr.h"
#include "../src/DN_RemovePoints.h"
#include "../src/helper_functions.h"

void mexFunction( int nlhs, mxArray *plhs[], 
      int nrhs, const mxArray*prhs[] )
     
{ 
    
    // check inputs and call feature C-function
M_wrapper_double( nlhs, plhs, nrhs, prhs, &DN_RemovePoints_absclose_05_ac2rat, 1);
    
    return;
    
}
