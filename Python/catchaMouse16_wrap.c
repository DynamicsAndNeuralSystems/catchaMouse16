#include <python2.7/Python.h>

// include functions
#include "../C/src/CO_AutoCorr.h"
#include "../C/src/CO_AddNoise.h"
#include "../C/src/CO_HistogramAMI.h"
#include "../C/src/CO_NonlinearAutocorr.h"
#include "../C/src/CO_TranslateShape.h"
#include "../C/src/IN_AutoMutualInfoStats.h"
#include "../C/src/DN_RemovePoints.h"
#include "../C/src/FC_LoopLocalSimple.h"
#include "../C/src/PH_Walker.h"
#include "../C/src/ST_LocalExtrema.h"
#include "../C/src/SY_DriftingMean.h"
#include "../C/src/SY_SlidingWindow.h"
#include "../C/src/SC_FluctAnal.h"
#include "../C/src/fft.h"
#include "../C/src/helper_functions.h"
#include "../C/src/histcounts.h"
#include "../C/src/stats.h"


// ---------------------------------------------------------------------
// -------------------- Main wrapper function --------------------------
// ---------------------------------------------------------------------

// used for all functions to wrap via a function handle

static PyObject * python_wrapper_double(PyObject * args, double (*f) (const double*, const int), int normalize)
{

    PyObject * py_tuple;
    int n;
    double * c_array;

    // parse arguments
    if(!PyArg_ParseTuple(args, "O", &py_tuple)){
       return NULL;
    }

    if (PyList_Check(py_tuple)){

        n = (int) PyList_Size(py_tuple);

        // allocate space for input array (C data format)
        c_array= malloc(n*sizeof(double));

        // write input array to c array
        int i;
        for (i=0; i<n; i++) {
            c_array[i] = (double) PyFloat_AsDouble(PyList_GetItem(py_tuple, i));
        }

    }
    else
    {
        if (PyTuple_Check(py_tuple)){

            n = (int) PyTuple_Size(py_tuple);

            // allocate space for input array (C data format)
            c_array= malloc(n*sizeof(double));

            // write input array to c array
            int i;
            for (i=0; i<n; i++) {
                c_array[i] = (double) PyFloat_AsDouble(PyTuple_GetItem(py_tuple, i));
            }

        }
        else{
            return NULL;
        }
    }

    // result variables
    double result;
    PyObject * ret;

    // copy input vector (just in case it is altered by the function)
    double * copy = malloc(n * sizeof * copy);    
    memcpy(copy, c_array, n * sizeof * copy);

    if (normalize){
        
        double * y_zscored = malloc(n * sizeof * y_zscored);
        zscore_norm2(copy, n, y_zscored);

        result = f(y_zscored, n);

        free(y_zscored);
    } 
    else {
        result = f(copy, n);
    }   

    //free(c_array);

    // build the resulting string into a Python object.
    ret = Py_BuildValue("d", result);

    return ret;
};

static PyObject * python_wrapper_int(PyObject * args, int (*f) (const double*, const int), int normalize)
{

    PyObject * py_tuple;
    int n;
    double * c_array;

    // parse arguments
    if(!PyArg_ParseTuple(args, "O", &py_tuple)){
       return NULL;
    }

    if (PyList_Check(py_tuple)){

        n = (int) PyList_Size(py_tuple);

        // allocate space for input array (C data format)
        c_array= malloc(n*sizeof(double));

        // write input array to c array
        int i;
        for (i=0; i<n; i++) {
            c_array[i] = (double) PyFloat_AsDouble(PyList_GetItem(py_tuple, i));
        }

    }
    else
    {
        if (PyTuple_Check(py_tuple)){

            n = (int) PyTuple_Size(py_tuple);

            // allocate space for input array (C data format)
            c_array= malloc(n*sizeof(double));

            // write input array to c array
            int i;
            for (i=0; i<n; i++) {
                c_array[i] = (double) PyFloat_AsDouble(PyTuple_GetItem(py_tuple, i));
            }

        }
        else{
            return NULL;
        }
    }

    // result variables
    int result;
    PyObject * ret;

    // copy input vector (just in case it is altered by the function)
    double * copy = malloc(n * sizeof * copy);    
    memcpy(copy, c_array, n * sizeof * copy);

    if (normalize){
        
        double * y_zscored = malloc(n * sizeof * y_zscored);
        zscore_norm2(copy, n, y_zscored);

        result = f(y_zscored, n);

        free(y_zscored);
    } 
    else {
        result = f(copy, n);
    }   

    //free(c_array);

    // build the resulting string into a Python object.
    ret = Py_BuildValue("n", result);

    return ret;
};


// ---------------------------------------------------------------------
// ----------------------- Functions to wrap ---------------------------
// ---------------------------------------------------------------------

static PyObject * SY_DriftingMean50_min_wrapper(PyObject * self, PyObject * args)
{
    return python_wrapper_double(args, &SY_DriftingMean50_min, 1);
}

static PyObject * DN_RemovePoints_absclose_05_ac2rat_wrapper(PyObject * self, PyObject * args)
{
    return python_wrapper_double(args, &DN_RemovePoints_absclose_05_ac2rat, 1);
}

static PyObject * AC_nl_036_wrapper(PyObject * self, PyObject * args)
{
    return python_wrapper_double(args, &AC_nl_036, 1);
}

static PyObject * AC_nl_112_wrapper(PyObject * self, PyObject * args)
{
    return python_wrapper_double(args, &AC_nl_112, 1);
}

static PyObject * ST_LocalExtrema_n100_diffmaxabsmin_wrapper(PyObject * self, PyObject * args)
{
    return python_wrapper_double(args, &ST_LocalExtrema_n100_diffmaxabsmin, 1);
}

static PyObject * IN_AutoMutualInfoStats_diff_20_gaussian_ami8_wrapper(PyObject * self, PyObject * args)
{
    return python_wrapper_double(args, &IN_AutoMutualInfoStats_diff_20_gaussian_ami8, 1);
}

static PyObject * CO_HistogramAMI_even_2_3_wrapper(PyObject * self, PyObject * args)
{
    return python_wrapper_double(args, &CO_HistogramAMI_even_2_3, 1);
}

static PyObject * CO_TranslateShape_circle_35_pts_statav4_m_wrapper(PyObject * self, PyObject * args)
{
    return python_wrapper_double(args, &CO_TranslateShape_circle_35_pts_statav4_m, 1);
}

static PyObject * CO_AddNoise_1_even_10_ami_at_10_wrapper(PyObject * self, PyObject * args)
{
    return python_wrapper_double(args, &CO_AddNoise_1_even_10_ami_at_10, 1);
}

static PyObject * PH_Walker_momentum_5_w_momentumzcross_wrapper(PyObject * self, PyObject * args)
{
    return python_wrapper_double(args, &PH_Walker_momentum_5_w_momentumzcross, 1);
}

static PyObject * SC_FluctAnal_2_dfa_50_2_logi_r2_se2_wrapper(PyObject * self, PyObject * args)
{
    return python_wrapper_double(args, &SC_FluctAnal_2_dfa_50_2_logi_r2_se2, 1);
}

static PyObject * PH_Walker_biasprop_05_01_sw_meanabsdiff_wrapper(PyObject * self, PyObject * args)
{
    return python_wrapper_double(args, &PH_Walker_biasprop_05_01_sw_meanabsdiff, 1);
}

static PyObject * CO_HistogramAMI_even_10_3_wrapper(PyObject * self, PyObject * args)
{
    return python_wrapper_double(args, &CO_HistogramAMI_even_10_3, 1);
}

static PyObject * AC_nl_035_wrapper(PyObject * self, PyObject * args)
{
    return python_wrapper_double(args, &AC_nl_035, 1);
}

static PyObject * FC_LoopLocalSimple_mean_stderr_chn_wrapper(PyObject * self, PyObject * args)
{
    return python_wrapper_double(args, &FC_LoopLocalSimple_mean_stderr_chn, 1);
}

static PyObject * CO_TranslateShape_circle_35_pts_std_wrapper(PyObject * self, PyObject * args)
{
    return python_wrapper_double(args, &CO_TranslateShape_circle_35_pts_std, 1);
}

// ---------------------------------------------------------------------
// ------------------------ Module creation ----------------------------
// ---------------------------------------------------------------------

// register within a module
static PyMethodDef catchaMouse16Methods[] = {
    { "SY_DriftingMean50_min", SY_DriftingMean50_min_wrapper, METH_VARARGS, "bla" },
    { "DN_RemovePoints_absclose_05_ac2rat", DN_RemovePoints_absclose_05_ac2rat_wrapper, METH_VARARGS, "bla" },
    { "AC_nl_036", AC_nl_036_wrapper, METH_VARARGS, "bla" },
    { "AC_nl_112", AC_nl_112_wrapper, METH_VARARGS, "bla" },
    { "ST_LocalExtrema_n100_diffmaxabsmin", ST_LocalExtrema_n100_diffmaxabsmin_wrapper, METH_VARARGS, "bla" },
    { "IN_AutoMutualInfoStats_diff_20_gaussian_ami8", IN_AutoMutualInfoStats_diff_20_gaussian_ami8_wrapper, METH_VARARGS, "bla" },
    { "CO_HistogramAMI_even_2_3", CO_HistogramAMI_even_2_3_wrapper, METH_VARARGS, "bla" },
    { "CO_TranslateShape_circle_35_pts_statav4_m", CO_TranslateShape_circle_35_pts_statav4_m_wrapper, METH_VARARGS, "bla" },
    { "CO_AddNoise_1_even_10_ami_at_10", CO_AddNoise_1_even_10_ami_at_10_wrapper, METH_VARARGS, "bla" },
    { "PH_Walker_momentum_5_w_momentumzcross", PH_Walker_momentum_5_w_momentumzcross_wrapper, METH_VARARGS, "bla" },
    { "SC_FluctAnal_2_dfa_50_2_logi_r2_se2", SC_FluctAnal_2_dfa_50_2_logi_r2_se2_wrapper, METH_VARARGS, "bla" },
    { "PH_Walker_biasprop_05_01_sw_meanabsdiff", PH_Walker_biasprop_05_01_sw_meanabsdiff_wrapper, METH_VARARGS, "bla" },
    { "CO_HistogramAMI_even_10_3", CO_HistogramAMI_even_10_3_wrapper, METH_VARARGS, "bla" },
    { "AC_nl_035", AC_nl_035_wrapper, METH_VARARGS, "bla" },
    { "FC_LoopLocalSimple_mean_stderr_chn", FC_LoopLocalSimple_mean_stderr_chn_wrapper, METH_VARARGS, "bla" },
    { "CO_TranslateShape_circle_35_pts_std", CO_TranslateShape_circle_35_pts_std_wrapper, METH_VARARGS, "bla" },
    { NULL, NULL, 0, NULL }
};


PyMODINIT_FUNC initcatchaMouse16_C(void)
{
    (void) Py_InitModule("catchaMouse16_C", catchaMouse16Methods);
}


int main(int argc, char *argv[])
{
    /* Pass argv[0] to the Python interpreter */
    Py_SetProgramName(argv[0]);
    
    /* Initialize the Python interpreter.  Required. */
    Py_Initialize();
    
    /* Add a static module */
    initcatchaMouse16_C();
    
}
