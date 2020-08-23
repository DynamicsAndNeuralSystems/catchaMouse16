#include <math.h>
#include "mex.h"

#include "M_wrapper.h"

#include "../C/src/CO_TranslateShape.h"
#include "../C/src/SY_SlidingWindow.h"
#include "../C/src/stats.h"

void mexFunction( int nlhs, mxArray *plhs[], 
      int nrhs, const mxArray*prhs[] )
     
{ 
    
    // check inputs and call feature C-function
M_wrapper_double( nlhs, plhs, nrhs, prhs, &CO_TranslateShape_circle_35_pts_statav4_m, 1);
    
    return;
    
}
