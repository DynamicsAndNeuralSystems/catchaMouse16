#include <math.h>
#include "mex.h"

#include "M_wrapper.h"

#include "../src/stats.h"
#include "../src/helper_functions.h"
#include "../src/CO_HistogramAMI.h"
#include "../src/histcounts.h"

void mexFunction( int nlhs, mxArray *plhs[], 
      int nrhs, const mxArray*prhs[] )
     
{ 
    
    // check inputs and call feature C-function
M_wrapper_double( nlhs, plhs, nrhs, prhs, &CO_HistogramAMI_even_2_3, 1);
    
    return;
    
}
