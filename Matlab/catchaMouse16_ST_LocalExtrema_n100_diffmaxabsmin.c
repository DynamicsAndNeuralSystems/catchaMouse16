#include <math.h>
#include "mex.h"

#include "M_wrapper.h"

#include "../C/src/ST_LocalExtrema.h"

void mexFunction( int nlhs, mxArray *plhs[], 
      int nrhs, const mxArray*prhs[] )
     
{ 
    
    // check inputs and call feature C-function
M_wrapper_double( nlhs, plhs, nrhs, prhs, &ST_LocalExtrema_n100_diffmaxabsmin, 1);
    
    return;
    
}
