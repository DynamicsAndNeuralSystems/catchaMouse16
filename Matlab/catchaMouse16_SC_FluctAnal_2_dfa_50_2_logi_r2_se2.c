#include <math.h>
#include "mex.h"

#include "M_wrapper.h"

#include "../src/stats.h"
#include "../src/CO_AutoCorr.h"
#include "../src/SC_FluctAnal.h"

void mexFunction( int nlhs, mxArray *plhs[], 
      int nrhs, const mxArray*prhs[] )
     
{ 
    
    // check inputs and call feature C-function
M_wrapper_double( nlhs, plhs, nrhs, prhs, &SC_FluctAnal_2_dfa_50_2_logi_r2_se2, 1);
    
    return;
    
}
