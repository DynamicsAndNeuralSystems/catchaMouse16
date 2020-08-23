#include <math.h>
#include "mex.h"

#include "M_wrapper.h"

#include "../C/src/CO_AutoCorr.h"
#include "../C/src/CO_NonlinearAutocorr.h"
#include "../C/src/fft.h"
#include "../C/src/helper_functions.h"
#include "../C/src/stats.h"

void mexFunction( int nlhs, mxArray *plhs[], 
      int nrhs, const mxArray*prhs[] )
     
{ 
    
    // check inputs and call feature C-function
M_wrapper_double( nlhs, plhs, nrhs, prhs, &AC_nl_036, 1);
    
    return;
    
}
