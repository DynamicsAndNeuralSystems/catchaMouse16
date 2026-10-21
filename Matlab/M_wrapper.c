#include "mex.h"
#include "../C/src/stats.h"
#include "../C/src/helper_functions.h"
#include "../C/src/CO_NonlinearAutocorr.h"

void M_wrapper_double( int nlhs, mxArray *plhs[], 
    int nrhs, const mxArray*prhs[], 
    double (*f) (const double*, const int), int normalize )
     
{ 
    double *inMatrix;       /* input vector (1xN or Nx1) */
    int nrows;
    double *outMatrix; /* output matrix */
    
    // check inputs
    if(nrhs != 1) {
        mexErrMsgIdAndTxt("catchaMouse16:nrhs",
                          "One input required.");
    }
    if(nlhs > 1) {
        mexErrMsgIdAndTxt("catchaMouse16:nlhs",
                          "One output required.");
    }
    if( !mxIsDouble(prhs[0]) || 
        mxIsComplex(prhs[0])) {
        mexErrMsgIdAndTxt("catchaMouse16:notDouble",
            "Input vector must be type double.");
    }
    // accept a row or a column vector (the data are contiguous either way)
    if(!mxIsEmpty(prhs[0]) && (mxGetNumberOfDimensions(prhs[0]) != 2 ||
       (mxGetM(prhs[0]) != 1 && mxGetN(prhs[0]) != 1))) {
        mexErrMsgIdAndTxt("catchaMouse16:notVector",
                          "Input must be a vector.");
    }
    
    // get input
    inMatrix = mxGetPr(prhs[0]);
    nrows = (int) mxGetNumberOfElements(prhs[0]);
    
    // set output
    plhs[0] = mxCreateDoubleMatrix(1,1,mxREAL);
    outMatrix = mxGetPr(plhs[0]);
    
    // an empty series has no features
    if (nrows == 0) {
        outMatrix[0] = mxGetNaN();
        return;
    }
    
    // calculate result
    if (normalize){
        
        double * y_zscored = malloc(nrows * sizeof * y_zscored);
        zscore_norm2(inMatrix, nrows, y_zscored);

        outMatrix[0] = f(y_zscored, nrows);

        free(y_zscored);
    } 
    else {
        outMatrix[0] = f(inMatrix, nrows);
    }   
    
    return;
    
}

void M_wrapper_int( int nlhs, mxArray *plhs[], 
    int nrhs, const mxArray*prhs[], 
    int (*f) (const double*, const int), int normalize )
     
{ 
    double *inMatrix;       /* input vector (1xN or Nx1) */
    int nrows;
    int *outMatrix; /* output matrix */
    
    // check inputs
    if(nrhs != 1) {
        mexErrMsgIdAndTxt("catchaMouse16:nrhs",
                          "One input required.");
    }
    if(nlhs > 1) {
        mexErrMsgIdAndTxt("catchaMouse16:nlhs",
                          "One output required.");
    }
    if( !mxIsDouble(prhs[0]) || 
        mxIsComplex(prhs[0])) {
        mexErrMsgIdAndTxt("catchaMouse16:notDouble",
            "Input vector must be type double.");
    }
    // accept a row or a column vector (the data are contiguous either way)
    if(!mxIsEmpty(prhs[0]) && (mxGetNumberOfDimensions(prhs[0]) != 2 ||
       (mxGetM(prhs[0]) != 1 && mxGetN(prhs[0]) != 1))) {
        mexErrMsgIdAndTxt("catchaMouse16:notVector",
                          "Input must be a vector.");
    }
    
    // get input
    inMatrix = mxGetPr(prhs[0]);
    nrows = (int) mxGetNumberOfElements(prhs[0]);
    
    // set output
    plhs[0] = mxCreateNumericMatrix(1,1, mxINT32_CLASS, mxREAL);
    outMatrix = (int*)mxGetData(plhs[0]);
    
    // calculate result
    if (normalize){
        
        double * y_zscored = malloc(nrows * sizeof * y_zscored);
        zscore_norm2(inMatrix, nrows, y_zscored);

        outMatrix[0] = f(y_zscored, nrows);

        free(y_zscored);
    } 
    else {
        outMatrix[0] = f(inMatrix, nrows);
    }   
    
    return;
    
}
