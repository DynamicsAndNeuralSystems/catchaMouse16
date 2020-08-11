#include <math.h>
#include "mex.h"

#include "M_wrapper.h"

#include "../src/CO_AutoCorr.h"
#include "../src/CO_NonlinearAutocorr.h"
#include "../src/fft.h"
#include "../src/helper_functions.h"
#include "../src/stats.h"

void mexFunction( int nlhs, mxArray *plhs[], 
      int nrhs, const mxArray*prhs[] )
     
{ 
    
    // check inputs and call feature C-function
M_wrapper_double( nlhs, plhs, nrhs, prhs, &AC_nl_112, 1);
    
    return;
    
}
