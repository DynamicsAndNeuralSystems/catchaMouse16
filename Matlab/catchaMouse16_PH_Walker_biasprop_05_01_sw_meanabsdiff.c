#include <math.h>
#include "mex.h"

#include "M_wrapper.h"

#include "../C/src/PH_Walker.h"
#include "../C/src/stats.h"
#include "../C/src/helper_functions.h"

void mexFunction( int nlhs, mxArray *plhs[], 
      int nrhs, const mxArray*prhs[] )
     
{ 
    
    // check inputs and call feature C-function
M_wrapper_double( nlhs, plhs, nrhs, prhs, &PH_Walker_biasprop_05_01_sw_meanabsdiff, 1);
    
    return;
    
}
