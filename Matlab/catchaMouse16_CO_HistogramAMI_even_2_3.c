#include <math.h>
#include "mex.h"

#include "M_wrapper.h"

#include "../C/src/stats.h"
#include "../C/src/helper_functions.h"
#include "../C/src/CO_HistogramAMI.h"
#include "../C/src/histcounts.h"

void mexFunction( int nlhs, mxArray *plhs[], 
      int nrhs, const mxArray*prhs[] )
     
{ 
    
    // check inputs and call feature C-function
M_wrapper_double( nlhs, plhs, nrhs, prhs, &CO_HistogramAMI_even_2_3, 1);
    
    return;
    
}
