#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <limits.h>
#include <math.h>
#include <string.h>

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
// ------------------------- Input conversion --------------------------
// ---------------------------------------------------------------------

// Is this buffer format a native double?
static int is_double_format(const char *format)
{
    return format != NULL && (strcmp(format, "d") == 0 || strcmp(format, "@d") == 0 ||
                              strcmp(format, "=d") == 0);
}

// Copy a one-dimensional sequence of real numbers (a list, tuple, NumPy
// array, array.array, or any other iterable) into a new C array of doubles.
// Returns 0 on success. On failure, returns -1 with a Python exception set
// (TypeError for input that is not a one-dimensional sequence of numbers).
static int to_double_array(PyObject *obj, double **out, int *n_out)
{
    double *y = NULL;
    Py_ssize_t n;

    // strings and bytes are sequences, but not of numbers
    if (PyUnicode_Check(obj) || PyBytes_Check(obj) || PyByteArray_Check(obj)) {
        PyErr_Format(PyExc_TypeError, "expected a sequence of numbers, not %.200s",
                     Py_TYPE(obj)->tp_name);
        return -1;
    }

    // objects that expose their data as a buffer (e.g., NumPy arrays)
    if (PyObject_CheckBuffer(obj)) {
        Py_buffer view;
        if (PyObject_GetBuffer(obj, &view, PyBUF_STRIDES | PyBUF_FORMAT) == 0) {
            if (view.ndim != 1) {
                int ndim = view.ndim;
                PyBuffer_Release(&view);
                PyErr_Format(PyExc_TypeError,
                             "expected a one-dimensional sequence of numbers, "
                             "got an array with %d dimensions", ndim);
                return -1;
            }
            if (is_double_format(view.format) && view.itemsize == sizeof(double)) {
                // copy the doubles directly
                n = view.shape[0];
                if (n > INT_MAX) {
                    PyBuffer_Release(&view);
                    PyErr_SetString(PyExc_OverflowError, "time series is too long");
                    return -1;
                }
                y = PyMem_Malloc((n > 0 ? n : 1) * sizeof *y);
                if (y == NULL) {
                    PyBuffer_Release(&view);
                    PyErr_NoMemory();
                    return -1;
                }
                for (Py_ssize_t i = 0; i < n; i++)
                    memcpy(&y[i], (const char *)view.buf + i * view.strides[0], sizeof(double));
                PyBuffer_Release(&view);
                *out = y;
                *n_out = (int)n;
                return 0;
            }
            // other element types (e.g., integer or float32 arrays) are
            // converted element by element below
            PyBuffer_Release(&view);
        } else {
            PyErr_Clear();
        }
    }

    // any other sequence or iterable: convert each element to a double
    PyObject *seq = PySequence_Fast(obj, "expected a sequence of numbers");
    if (seq == NULL)
        return -1;
    n = PySequence_Fast_GET_SIZE(seq);
    if (n > INT_MAX) {
        Py_DECREF(seq);
        PyErr_SetString(PyExc_OverflowError, "time series is too long");
        return -1;
    }
    y = PyMem_Malloc((n > 0 ? n : 1) * sizeof *y);
    if (y == NULL) {
        Py_DECREF(seq);
        PyErr_NoMemory();
        return -1;
    }
    PyObject **items = PySequence_Fast_ITEMS(seq);
    for (Py_ssize_t i = 0; i < n; i++) {
        y[i] = PyFloat_AsDouble(items[i]);
        if (y[i] == -1.0 && PyErr_Occurred()) {
            if (PyErr_ExceptionMatches(PyExc_TypeError)) {
                PyErr_Format(PyExc_TypeError,
                             "expected a sequence of numbers, but element %zd is %.200s",
                             i, Py_TYPE(items[i])->tp_name);
            }
            PyMem_Free(y);
            Py_DECREF(seq);
            return -1;
        }
    }
    Py_DECREF(seq);
    *out = y;
    *n_out = (int)n;
    return 0;
}


// ---------------------------------------------------------------------
// -------------------- Main wrapper function --------------------------
// ---------------------------------------------------------------------

// used for all functions to wrap via a function handle

static PyObject * python_wrapper_double(PyObject * args, double (*f) (const double*, const int), int normalize)
{
    PyObject * input;
    double * y;
    int n;
    double result;

    // parse arguments
    if (!PyArg_ParseTuple(args, "O", &input))
        return NULL;
    if (to_double_array(input, &y, &n) != 0)
        return NULL;

    // an empty series has no features
    if (n == 0) {
        PyMem_Free(y);
        return PyFloat_FromDouble(NAN);
    }

    if (normalize) {
        double * y_zscored = PyMem_Malloc(n * sizeof * y_zscored);
        if (y_zscored == NULL) {
            PyMem_Free(y);
            return PyErr_NoMemory();
        }
        zscore_norm2(y, n, y_zscored);
        result = f(y_zscored, n);
        PyMem_Free(y_zscored);
    }
    else {
        result = f(y, n);
    }

    PyMem_Free(y);

    return PyFloat_FromDouble(result);
}

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


static struct PyModuleDef catchaMouse16_Cmodule = {
    PyModuleDef_HEAD_INIT,
    "catchaMouse16_C",   /* name of module */
    "16 Canonical Time-series CHaracteristcs for Mouse fMRI", /* module documentation, may be NULL */
    -1,       /* size of per-interpreter state of the module,
                 or -1 if the module keeps state in global variables. */
    catchaMouse16Methods,
};

PyMODINIT_FUNC PyInit_catchaMouse16_C(void)
{
    return PyModule_Create(&catchaMouse16_Cmodule);
}
