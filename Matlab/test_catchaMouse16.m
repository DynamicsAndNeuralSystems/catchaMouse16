% test_catchaMouse16  Check the catchaMouse16 MEX functions.
%
% Run mexAll first, then run this script (from any folder). It errors on
% the first failed check.
%
% Feature values are checked against the hctsa v1.03 reference values used in
% C/tests/test_features.c (computed in MATLAB R2026a on the z-scored series).
% CO_AddNoise (different random number generator to MATLAB) and the
% SC_FluctAnal standard error (GSL's robust fit vs MATLAB's robustfit) cannot
% match hctsa closely, so only their NaN behavior is checked.

testDir = fileparts(mfilename('fullpath'));
addpath(testDir);
repoDir = fullfile(testDir, '..');
dataDir = fullfile(repoDir, 'C', 'tests', 'data');
numChecks = 0;

% Same tolerance as the C tests: relative 1e-9 (absolute 1e-12 near 0)
isClose = @(value, expected) (isnan(expected) && isnan(value)) || ...
    (~isnan(value) && abs(value - expected) <= 1e-9 * max(abs(expected), 1e-3));

% hctsa v1.03 values for test_ts.csv (N = 900)
reference = {
    'AC_nl_035', 0.029388510653943489
    'AC_nl_036', -0.089862158681074611
    'AC_nl_112', 0.95527350930096855
    'CO_HistogramAMI_even_2_3', 0.0064026521213012694
    'CO_HistogramAMI_even_10_3', 0.069437467855260393
    'IN_AutoMutualInfoStats_diff_20_gaussian_ami8', 0.0017376086763743674
    'DN_RemovePoints_absclose_05_ac2rat', 0.36055461984180104
    'CO_TranslateShape_circle_35_pts_std', 0.81476734130849149
    'CO_TranslateShape_circle_35_pts_statav4_m', 0.063948212366642623
    'SY_DriftingMean50_min', -0.17515608004321692
    'PH_Walker_biasprop_05_01_sw_meanabsdiff', 0.79368904259375106
    'PH_Walker_momentum_5_w_momentumzcross', 0.15016685205784205
    'FC_LoopLocalSimple_mean_stderr_chn', 0.090009471873276056
    'ST_LocalExtrema_n100_diffmaxabsmin', 0.78721699565949121
};

x = readmatrix(fullfile(repoDir, 'test_ts.csv'));
assert(iscolumn(x) && length(x) == 900, 'test_ts.csv should be 900 values');

% --- single features: reference values, for column and row vectors ---
for i = 1:size(reference, 1)
    name = reference{i, 1};
    fh = str2func(['catchaMouse16_', name]);
    valueColumn = fh(x);
    valueRow = fh(x');
    assert(isClose(valueColumn, reference{i, 2}), ...
        'test_ts %s: got %.17g, expected %.17g', name, valueColumn, reference{i, 2});
    assert(isequal(valueRow, valueColumn), 'test_ts %s: row and column vectors differ', name);
    numChecks = numChecks + 2;
end

% --- catchaMouse16_all: row and column vectors give the same values ---
[valuesColumn, names] = catchaMouse16_all(x);
valuesRow = catchaMouse16_all(x');
assert(isequal(size(valuesColumn), [16, 1]) && length(names) == 16, 'expected 16 features');
assert(isequaln(valuesRow, valuesColumn), 'catchaMouse16_all: row and column vectors differ');
featureList = strsplit(strtrim(fileread(fullfile(repoDir, 'featureList.txt'))));
assert(isequal(sort(names(:)), sort(featureList(:))), 'feature names differ from featureList.txt');
for i = 1:size(reference, 1)
    value = valuesColumn(strcmp(names, reference{i, 1}));
    assert(isClose(value, reference{i, 2}), 'catchaMouse16_all %s: got %.17g, expected %.17g', ...
        reference{i, 1}, value, reference{i, 2});
end
% cannot match hctsa closely, but should be a number at N = 900
assert(all(isfinite(valuesColumn)), 'catchaMouse16_all: expected 16 finite values at N = 900');
numChecks = numChecks + 4;

% --- other hctsa v1.03 reference values from C/tests/test_features.c ---
otherReference = {
    'wn299.txt', 'ST_LocalExtrema_n100_diffmaxabsmin', 0.91471424058108486
    'rw437.txt', 'ST_LocalExtrema_n100_diffmaxabsmin', 0.97902825419057138
    'int1000.txt', 'DN_RemovePoints_absclose_05_ac2rat', 0.21171499248284334
    'wn150.txt', 'SC_FluctAnal_2_dfa_50_2_logi_r2_se2', NaN
};
for i = 1:size(otherReference, 1)
    y = readmatrix(fullfile(dataDir, otherReference{i, 1}), 'FileType', 'text');
    fh = str2func(['catchaMouse16_', otherReference{i, 2}]);
    value = fh(y);
    assert(isClose(value, otherReference{i, 3}), '%s %s: got %.17g, expected %.17g', ...
        otherReference{i, 1}, otherReference{i, 2}, value, otherReference{i, 3});
    numChecks = numChecks + 1;
end

% --- short series: NaN rather than a crash ---
values8 = catchaMouse16_all(x(1:8));
assert(isequal(size(values8), [16, 1]), 'N = 8: expected 16 values');
assert(isnan(catchaMouse16_FC_LoopLocalSimple_mean_stderr_chn(x(1:8))), 'N = 8: expected NaN');
assert(all(isnan(catchaMouse16_all([]))), 'empty input: expected NaN');
numChecks = numChecks + 3;

% --- bad input raises an error ---
badInputs = {zeros(900, 2), 'abc', complex(x, 1)};
for i = 1:length(badInputs)
    threw = false;
    try
        catchaMouse16_all(badInputs{i});
    catch
        threw = true;
    end
    assert(threw, 'catchaMouse16_all: bad input %d did not raise an error', i);
    threw = false;
    try
        catchaMouse16_AC_nl_035(badInputs{i});
    catch
        threw = true;
    end
    assert(threw, 'catchaMouse16_AC_nl_035: bad input %d did not raise an error', i);
    numChecks = numChecks + 2;
end

fprintf('All %d catchaMouse16 MATLAB checks passed.\n', numChecks);
