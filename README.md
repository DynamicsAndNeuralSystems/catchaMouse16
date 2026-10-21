<p align="center"><img src="img/catch_a_mouse16_vecorized.svg" alt="catchaMouse16 logo" height="220"/></p>

<h1 align="center"><em>catchaMouse16</em>: CAnonical Time-series CHaracteristics for fMRI (trained on mouse data)</h1>

<p align="center">
  <a href="https://doi.org/10.52294/001c.140433"><img src="https://img.shields.io/badge/paper-Aperture%20Neuro%20(2025)-informational" alt="Paper"/></a>
  <a href="https://www.gnu.org/licenses/gpl-3.0"><img src="https://img.shields.io/badge/license-GPLv3-blue.svg" alt="License: GPL v3"/></a>
  <a href="https://time-series-features.gitbook.io/time-series-analysis-tools/feature-subsets/catchamouse16"><img src="https://img.shields.io/badge/docs-GitBook-blue" alt="Documentation"/></a>
  <a href="https://twitter.com/compTimeSeries"><img src="https://img.shields.io/twitter/url/https/twitter.com/compTimeSeries.svg?style=social&label=Follow%20%40compTimeSeries" alt="Follow @compTimeSeries"/></a>
</p>

_catchaMouse16_ is a set of 16 time-series features, implemented in C and available in Python and MATLAB (and Julia, via a separate package).
The features are a subset of those in the [_hctsa_](https://github.com/benfulcher/hctsa/) toolbox, selected for their classification performance across a collection of mouse fMRI time-series classification problems.
See the [pipeline and paper-specific repository](https://github.com/DynamicsAndNeuralSystems/Catchamouse16_paper_code) for details.

📖 **Full documentation:** [time-series-features.gitbook.io](https://time-series-features.gitbook.io/time-series-analysis-tools/feature-subsets/catchamouse16), covering installation in each language, the theory behind and behavior of each feature, and publications that use _catchaMouse16_.

## Installation

Download the source code from this repository and install [GSL](https://www.gnu.org/software/gsl/) (the GNU Scientific Library, needed on Unix-like systems).
Then build for your language of choice.

| Language | Where | How to build |
|---|---|---|
| C | [`C/`](C) | run `make` inside `C/` |
| Python | [`Python/`](Python) | `python3 setup_P3.py build` then `python3 setup_P3.py install` |
| MATLAB | [`Matlab/`](Matlab) | run `mexAll` inside `Matlab/` (it looks for GSL in `/usr/local`; edit the paths in `mexAll.m` if GSL is installed elsewhere, e.g., `/opt/homebrew` on Apple silicon) |
| Julia | [CatchaMouse16.jl](https://github.com/brendanjohnharris/CatchaMouse16.jl) | `] add CatchaMouse16` (pre-built binaries for all platforms) |

The Python build uses `distutils` (provided by setuptools on recent Python versions).
Some features return `NaN` (and print an error message) for very short series; for example, `ST_LocalExtrema_n100_diffmaxabsmin` returns `NaN` for series of 100 and 150 samples.
The mouse fMRI series in the paper have 900 samples.
A Python 2 build is also available through `setup.py`; see the [documentation](https://time-series-features.gitbook.io/time-series-analysis-tools/feature-subsets/catchamouse16) for details.

## Quick start

In Python (the features take a list of floats; a NumPy array must be converted with `.tolist()`):

```python
import numpy as np
import catchaMouse16

x = np.random.randn(900).tolist()  # your time series
print(catchaMouse16.SC_FluctAnal_2_dfa_50_2_logi_r2_se2(x))  # a single feature
out = catchaMouse16.catchaMouse16_all(x)  # a dict with 'names' and 'values'
```

Run `python3 test_features.py` inside `Python/` to compute all 16 features for the example series [`test_ts.csv`](test_ts.csv).

In MATLAB (after running `mexAll`):

```matlab
x = randn(900, 1);                                 % a column vector
f = catchaMouse16_AC_nl_035(x);                    % a single feature (needs a column vector)
[featureValues, featureNames] = catchaMouse16_all(x');  % all 16 features (needs a row vector)
```

Individual features are available as `catchaMouse16_{feature}`.
Note that `catchaMouse16_all` currently requires a row vector, whereas the individual feature functions require a column vector.

From the command line (after running `make` inside `C/`), compute all features for the time series in `<infile>`:

```bash
./run_feat <infile> <outfile>
```

The input is one value per line.
Each line of the output is a feature value, its name, and its computation time in milliseconds.
If no `<outfile>` is given, the output is sent to standard output.

In Julia:

```julia
using CatchaMouse16
X = randn(1000, 10)  # 10 time series
F = catchaMouse16(X)  # a 16×10 matrix of features
```

## The features

All _catchaMouse16_ features are computed on the _z_-scored time series, so they capture the time-ordering of the data and are insensitive to its mean and variance.

| Feature name | Description |
|---|---|
| `AC_nl_035` | Nonlinear autocorrelation, ⟨x<sub>t</sub>² x<sub>t−3</sub> x<sub>t−5</sub>⟩<sub>t</sub> |
| `AC_nl_036` | Nonlinear autocorrelation, ⟨x<sub>t</sub>² x<sub>t−3</sub> x<sub>t−6</sub>⟩<sub>t</sub> |
| `AC_nl_112` | Nonlinear autocorrelation, ⟨x<sub>t</sub> x<sub>t−1</sub>² x<sub>t−2</sub>⟩<sub>t</sub> |
| `CO_HistogramAMI_even_10_3` | Automutual information at lag 3 using a 10-bin histogram estimator |
| `CO_HistogramAMI_even_2_3` | Automutual information at lag 3 using a 2-bin histogram estimator |
| `IN_AutoMutualInfoStats_diff_20_gaussian_ami8` | Mutual information at time lag 8 using a Gaussian estimator |
| `SC_FluctAnal_2_dfa_50_2_logi_r2_se2` | Timescale–fluctuation curve using detrended fluctuation analysis (DFA) |
| `CO_AddNoise_1_even_10_ami_at_10` | Automutual information at lag 1 after adding white noise at an SNR of 1 |
| `FC_LoopLocalSimple_mean_stderr_chn` | Change in prediction error from a mean forecaster using prior windows of data |
| `CO_TranslateShape_circle_35_pts_std` | Variability of time-series points inside a circle translated across the time domain |
| `PH_Walker_momentum_5_w_momentumzcross` | Statistics of a simulated mechanical particle driven by the time series |
| `PH_Walker_biasprop_05_01_sw_meanabsdiff` | Statistics of a simulated mechanical particle driven by the time series |
| `SY_DriftingMean50_min` | Minimum mean across 50-sample windows, divided by the mean variance in the windows |
| `CO_TranslateShape_circle_35_pts_statav4_m` | StatAv of the statistics of local time-series shapes |
| `DN_RemovePoints_absclose_05_ac2rat` | Change in lag-2 autocorrelation from removing 50% of the time-series values closest to the mean |
| `ST_LocalExtrema_n100_diffmaxabsmin` | Asymmetry in extreme local events |

The list of names is also in [`featureList.txt`](featureList.txt).
The [documentation](https://time-series-features.gitbook.io/time-series-analysis-tools/feature-subsets/catchamouse16) gives the interpretable name used in the paper for each feature.

## Minimum time-series length

Each feature returns `NaN` when the series is too short to compute it.
The table gives the shortest length N at which each feature returns a value.
These lengths were measured with the C code on white noise, AR(1) (φ = 0.9) and random-walk series (50 draws of each at every N from 1 to 1000), and agree with the length checks in [`C/src`](C/src).
A feature that returns a value at these lengths is not necessarily meaningful there.

| Feature name | Minimum N |
|---|---|
| `AC_nl_035` | 6 |
| `AC_nl_036` | 7 |
| `AC_nl_112` | 3 |
| `CO_HistogramAMI_even_10_3` | 4 |
| `CO_HistogramAMI_even_2_3` | 4 |
| `IN_AutoMutualInfoStats_diff_20_gaussian_ami8` | 14 |
| `SC_FluctAnal_2_dfa_50_2_logi_r2_se2` | 34 (data-dependent above this; see below) |
| `CO_AddNoise_1_even_10_ami_at_10` | 2 |
| `FC_LoopLocalSimple_mean_stderr_chn` | 12 |
| `CO_TranslateShape_circle_35_pts_std` | 8 |
| `PH_Walker_momentum_5_w_momentumzcross` | 2 |
| `PH_Walker_biasprop_05_01_sw_meanabsdiff` | 2 |
| `SY_DriftingMean50_min` | 50 |
| `CO_TranslateShape_circle_35_pts_statav4_m` | 10 (data-dependent above this; see below) |
| `DN_RemovePoints_absclose_05_ac2rat` | 5 |
| `ST_LocalExtrema_n100_diffmaxabsmin` | 200 |

Two features can also return `NaN` at longer lengths, depending on the data (_hctsa_ returns `NaN` in the same cases):

- `SC_FluctAnal_2_dfa_50_2_logi_r2_se2` returns `NaN` when the fitted break point leaves fewer than 8 timescales for the second linear fit.
  In the draws above, this happened for 40–50% of series at N = 34–50, 20–30% at N = 101–200, and 1–9% at N = 701–1000 (most often for random walks).
- `CO_TranslateShape_circle_35_pts_statav4_m` returns `NaN` when every point has the same number of neighbors inside the circle, which happens for smooth series.
  It was `NaN` for almost all random walks at every length, for about 13% of AR(1) series at N = 10–200 (under 1% above N = 700), and for white noise only at N ≤ 18.

To compute the full set, use at least 200 samples, because `ST_LocalExtrema_n100_diffmaxabsmin` is always `NaN` below that.
Even then, expect occasional `NaN` values from the two features above.

## Differences from _hctsa_

On continuous-valued series longer than 15 samples, 14 of the 16 features match the _hctsa_ v1.03 code used in the paper to within floating-point rounding (relative differences below 1e-10).
Two features cannot match it exactly:

- `CO_AddNoise_1_even_10_ami_at_10` draws its noise with GSL's MT19937 generator and ziggurat Gaussian sampler, whereas _hctsa_ uses MATLAB's `randn`, so the noise and the feature value differ.
  The generator is seeded with 0 on every call (which GSL's MT19937 maps to its default seed, 4357), so the value is deterministic within this package: the same series always gives the same value.
  Changing the random seed alone changes this feature by about 10–15% (coefficient of variation at N = 150–1000), and values differed from _hctsa_'s by a similar amount (13–26% for N ≥ 250).
- `SC_FluctAnal_2_dfa_50_2_logi_r2_se2` computes the standard error of a robust linear fit with GSL's robust regression, which differs from MATLAB's `robustfit` by about 0.03–1.6%.
  Its `NaN` cases match _hctsa_.
  When the robust fit does not converge (about 0.5% of random series), both return a value, and the difference can reach about 6%.

Discrete-valued (e.g., integer) series can also differ from _hctsa_ in two features.
In `CO_HistogramAMI_even_2_3`, a data value can fall exactly on the middle bin edge, so its bin depends on the last bit of the z-scored value, and the C z-scoring is not bitwise identical to MATLAB's `zscore`.
`ST_LocalExtrema_n100_diffmaxabsmin` keeps a full final window whose last z-scored sample is exactly 0, which _hctsa_ v1.03 discards as a side effect of its zero-padding check.

Two features also return a value for some very short series where _hctsa_ v1.03 returns `NaN`.
`FC_LoopLocalSimple_mean_stderr_chn` returns a value at N = 12–14; _hctsa_ returns `NaN` there only because a different output of the same operation fails to compute.
`IN_AutoMutualInfoStats_diff_20_gaussian_ami8` returns a value at N = 14–15.

## Citing _catchaMouse16_

If you use _catchaMouse16_, please cite the open-access paper:

> I. Alam, B. Harris, P. Cahill, O. Cliff, M. Markicevic, V. Zerbi, B. D. Fulcher.
> [Canonical time-series features for characterizing biologically informative dynamical patterns in fMRI](https://doi.org/10.52294/001c.140433).
> _Aperture Neuro_ (2025).

<details>
<summary>BibTeX</summary>

```bibtex
@article{alam2025catchamouse16,
  title   = {Canonical time-series features for characterizing biologically informative dynamical patterns in {fMRI}},
  author  = {Alam, Imran and Harris, Brendan and Cahill, Patrick and Cliff, Oliver and Markicevic, Marija and Zerbi, Valerio and Fulcher, Ben D.},
  journal = {Aperture Neuro},
  volume  = {5},
  year    = {2025},
  doi     = {10.52294/001c.140433}
}
```

</details>

## Contributing

Please feel free to ask any question in the [Issues](https://github.com/DynamicsAndNeuralSystems/catchaMouse16/issues).
Suggestions and improvements are welcome through [pull requests](https://github.com/DynamicsAndNeuralSystems/catchaMouse16/pulls).

## License

_catchaMouse16_ is licensed under the [GNU GPL v3](http://www.gnu.org/licenses/gpl-3.0.html) (or later).
