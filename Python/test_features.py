"""Tests for the catchaMouse16 Python wrapper.

Feature values are checked against the hctsa v1.03 reference values used in
C/tests/test_features.c (computed in MATLAB R2026a on the z-scored series).
CO_AddNoise (different random number generator to MATLAB) and the
SC_FluctAnal standard error (GSL's robust fit vs MATLAB's robustfit) cannot
match hctsa closely, so only their NaN behavior is checked.

Run with `python test_features.py` or `python -m pytest test_features.py`
(inside Python/, after building the extension).
"""

import math
import os

import numpy as np

import catchaMouse16

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(HERE)
TEST_TS = os.path.join(REPO, "test_ts.csv")
C_TEST_DATA = os.path.join(REPO, "C", "tests", "data")

FEATURE_NAMES = [
    "SY_DriftingMean50_min",
    "DN_RemovePoints_absclose_05_ac2rat",
    "AC_nl_036",
    "AC_nl_112",
    "ST_LocalExtrema_n100_diffmaxabsmin",
    "IN_AutoMutualInfoStats_diff_20_gaussian_ami8",
    "CO_HistogramAMI_even_2_3",
    "CO_TranslateShape_circle_35_pts_statav4_m",
    "CO_AddNoise_1_even_10_ami_at_10",
    "PH_Walker_momentum_5_w_momentumzcross",
    "SC_FluctAnal_2_dfa_50_2_logi_r2_se2",
    "PH_Walker_biasprop_05_01_sw_meanabsdiff",
    "CO_HistogramAMI_even_10_3",
    "AC_nl_035",
    "FC_LoopLocalSimple_mean_stderr_chn",
    "CO_TranslateShape_circle_35_pts_std",
]

# hctsa v1.03 values for test_ts.csv (N = 900), as in C/tests/test_features.c
TEST_TS_REFERENCE = {
    "AC_nl_035": 0.029388510653943489,
    "AC_nl_036": -0.089862158681074611,
    "AC_nl_112": 0.95527350930096855,
    "CO_HistogramAMI_even_2_3": 0.0064026521213012694,
    "CO_HistogramAMI_even_10_3": 0.069437467855260393,
    "IN_AutoMutualInfoStats_diff_20_gaussian_ami8": 0.0017376086763743674,
    "DN_RemovePoints_absclose_05_ac2rat": 0.36055461984180104,
    "CO_TranslateShape_circle_35_pts_std": 0.81476734130849149,
    "CO_TranslateShape_circle_35_pts_statav4_m": 0.063948212366642623,
    "SY_DriftingMean50_min": -0.17515608004321692,
    "PH_Walker_biasprop_05_01_sw_meanabsdiff": 0.79368904259375106,
    "PH_Walker_momentum_5_w_momentumzcross": 0.15016685205784205,
    "FC_LoopLocalSimple_mean_stderr_chn": 0.090009471873276056,
    "ST_LocalExtrema_n100_diffmaxabsmin": 0.78721699565949121,
}

# Other hctsa v1.03 reference values from C/tests/test_features.c:
# (data file in C/tests/data, feature, expected value; NaN means NaN expected)
OTHER_REFERENCE = [
    ("wn299.txt", "ST_LocalExtrema_n100_diffmaxabsmin", 0.91471424058108486),
    ("rw437.txt", "ST_LocalExtrema_n100_diffmaxabsmin", 0.97902825419057138),
    ("int1000.txt", "DN_RemovePoints_absclose_05_ac2rat", 0.21171499248284334),
    ("wn150.txt", "SC_FluctAnal_2_dfa_50_2_logi_r2_se2", math.nan),
]


def load_test_ts():
    return np.loadtxt(TEST_TS)


def assert_close(value, expected, label):
    """Same tolerance as the C tests: relative 1e-9 (absolute 1e-12 near 0)."""
    assert isinstance(value, float), "%s: expected a float, got %r" % (label, value)
    if math.isnan(expected):
        assert math.isnan(value), "%s: got %r, expected NaN" % (label, value)
    else:
        tol = 1e-9 * max(abs(expected), 1e-3)
        assert not math.isnan(value) and abs(value - expected) <= tol, (
            "%s: got %.17g, expected %.17g" % (label, value, expected))


def assert_raises_type_error(fun, arg, label):
    try:
        fun(arg)
    except TypeError:
        return
    except Exception as err:
        raise AssertionError("%s: raised %s (%s), expected TypeError"
                             % (label, type(err).__name__, err))
    raise AssertionError("%s: no error, expected TypeError" % label)


def same_values(a, b):
    """Elementwise equal, treating NaN as equal to NaN."""
    return len(a) == len(b) and all(
        (math.isnan(x) and math.isnan(y)) or x == y for x, y in zip(a, b))


def test_feature_names():
    out = catchaMouse16.catchaMouse16_all(load_test_ts())
    assert out["names"] == FEATURE_NAMES
    with open(os.path.join(REPO, "featureList.txt")) as f:
        assert sorted(f.read().split()) == sorted(FEATURE_NAMES)
    for name in FEATURE_NAMES:
        assert callable(getattr(catchaMouse16, name)), name


def test_reference_values_test_ts():
    x = load_test_ts()
    assert len(x) == 900
    for name, expected in TEST_TS_REFERENCE.items():
        assert_close(getattr(catchaMouse16, name)(x.tolist()), expected, "test_ts " + name)
    # cannot match hctsa closely, but should be a number at N = 900
    for name in ["CO_AddNoise_1_even_10_ami_at_10", "SC_FluctAnal_2_dfa_50_2_logi_r2_se2"]:
        value = getattr(catchaMouse16, name)(x)
        assert isinstance(value, float) and math.isfinite(value), (name, value)


def test_reference_values_other_series():
    for filename, name, expected in OTHER_REFERENCE:
        x = np.loadtxt(os.path.join(C_TEST_DATA, filename))
        assert_close(getattr(catchaMouse16, name)(x), expected, filename + " " + name)


def test_catchaMouse16_all_matches_single_features():
    x = load_test_ts()
    out = catchaMouse16.catchaMouse16_all(x)
    single = [getattr(catchaMouse16, name)(x) for name in FEATURE_NAMES]
    assert same_values(out["values"], single)
    for name, value in zip(out["names"], out["values"]):
        if name in TEST_TS_REFERENCE:
            assert_close(value, TEST_TS_REFERENCE[name], "catchaMouse16_all " + name)


def test_input_types_give_identical_values():
    x = load_test_ts()
    reference = [getattr(catchaMouse16, name)(x.tolist()) for name in FEATURE_NAMES]
    inputs = {
        "NumPy array": x,
        "tuple": tuple(x.tolist()),
        "list of NumPy floats": list(x),
        "non-contiguous NumPy array": np.repeat(x, 2)[::2],
        "generator": (v for v in x.tolist()),
    }
    for label, data in inputs.items():
        if label != "generator":
            values = [getattr(catchaMouse16, name)(data) for name in FEATURE_NAMES]
            assert same_values(values, reference), label
        out = catchaMouse16.catchaMouse16_all(data)
        assert same_values(out["values"], reference), "catchaMouse16_all " + label

    # integer input is converted to floating point
    xi = np.arange(1, 901) % 7
    assert catchaMouse16.AC_nl_035(xi) == catchaMouse16.AC_nl_035([float(v) for v in xi])


def test_bad_input_raises_type_error():
    bad_inputs = {
        "string": "1 2 3",
        "None": None,
        "scalar": 5.0,
        "list with a string": [1.0, 2.0, "3"],
        "list with None": [1.0, None, 3.0],
        "nested list": [[1.0, 2.0], [3.0, 4.0]],
        "2-D NumPy array": np.zeros((900, 1)),
        "NumPy string array": np.array(["a", "b", "c"]),
    }
    for label, data in bad_inputs.items():
        assert_raises_type_error(catchaMouse16.AC_nl_035, data, "AC_nl_035 " + label)
        assert_raises_type_error(catchaMouse16.catchaMouse16_all, data,
                                 "catchaMouse16_all " + label)


def test_short_series_give_nan_not_a_crash():
    # N = 8: FC_LoopLocalSimple used to crash (training length > N)
    x8 = load_test_ts()[:8]
    out = catchaMouse16.catchaMouse16_all(x8)
    assert len(out["values"]) == 16
    assert all(isinstance(v, float) for v in out["values"])
    assert math.isnan(catchaMouse16.FC_LoopLocalSimple_mean_stderr_chn(x8))
    # features that need more than 8 samples return NaN
    for name in ["SY_DriftingMean50_min", "ST_LocalExtrema_n100_diffmaxabsmin",
                 "SC_FluctAnal_2_dfa_50_2_logi_r2_se2"]:
        assert math.isnan(getattr(catchaMouse16, name)(x8)), name
    # an empty series gives NaN for every feature
    for data in ([], np.array([])):
        out = catchaMouse16.catchaMouse16_all(data)
        assert len(out["values"]) == 16 and all(math.isnan(v) for v in out["values"])


if __name__ == "__main__":
    tests = [(name, fun) for name, fun in sorted(globals().items())
             if name.startswith("test_") and callable(fun)]
    for name, fun in tests:
        fun()
        print("passed:", name)
    print("All %d tests passed." % len(tests))
