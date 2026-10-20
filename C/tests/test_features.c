// Value checks for the catchaMouse16 C features.
//
// Reference values were computed with hctsa v1.03 (the version used in the
// catchaMouse16 paper) in MATLAB R2026a, on z-scored input, using the same
// operations as the catchaMouse16 features (e.g. ST_LocalExtrema(x_z,'n',100),
// SC_FluctAnal(x_z,2,'dfa',50,2,[],1)).
// CO_AddNoise (different random number generator to MATLAB) and the
// SC_FluctAnal standard error (GSL's robust fit vs MATLAB's robustfit differ
// by ~1%) cannot match hctsa to high precision, so only their NaN behavior is
// checked here.
//
// Series in tests/data (seeded NumPy draws, written with %.17g):
//   wn299.txt, wn150.txt: white noise; rw437.txt: random walk;
//   int1000.txt: integers 0-4 (many tied values).
//
// Usage: run_tests <data directory> <path to test_ts.csv>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "CO_AutoCorr.h"
#include "CO_NonlinearAutocorr.h"
#include "SY_DriftingMean.h"
#include "ST_LocalExtrema.h"
#include "DN_RemovePoints.h"
#include "IN_AutoMutualInfoStats.h"
#include "PH_Walker.h"
#include "CO_TranslateShape.h"
#include "SC_FluctAnal.h"
#include "FC_LoopLocalSimple.h"
#include "CO_HistogramAMI.h"
#include "stats.h"

static int n_fail = 0, n_pass = 0;

// Read whitespace-separated values from a file, then z-score them
static double *read_zscored(const char *path, int *size)
{
    FILE *f = fopen(path, "r");
    if (f == NULL) {
        fprintf(stderr, "Can't open %s\n", path);
        exit(2);
    }
    int capacity = 1024, n = 0;
    double value, *y = malloc(capacity * sizeof *y);
    while (fscanf(f, "%lf", &value) == 1) {
        if (n == capacity) {
            capacity *= 2;
            y = realloc(y, capacity * sizeof *y);
        }
        y[n++] = value;
    }
    fclose(f);
    double *y_z = malloc(n * sizeof *y_z);
    zscore_norm2(y, n, y_z);
    free(y);
    *size = n;
    return y_z;
}

// Check a feature value against a reference (NAN means NaN is expected)
static void check(const char *series, const char *feature, double value, double expected)
{
    int ok;
    if (isnan(expected))
        ok = isnan(value);
    else
        ok = !isnan(value) && fabs(value - expected) <= 1e-9 * fmax(fabs(expected), 1e-3);
    if (ok) {
        n_pass++;
    } else {
        n_fail++;
        printf("FAIL %s %s: got %.17g, expected %.17g\n", series, feature, value, expected);
    }
}

static char *join(const char *dir, const char *file)
{
    static char path[4096];
    snprintf(path, sizeof path, "%s/%s", dir, file);
    return path;
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <data directory> <path to test_ts.csv>\n", argv[0]);
        return 2;
    }
    // unbuffered, so that failures are reported even if a later check crashes
    setvbuf(stdout, NULL, _IONBF, 0);
    const char *dir = argv[1];
    int N;
    double *y;

    // --- test_ts.csv (N = 900, the paper's series length): all features
    // that can match hctsa v1.03 exactly ---
    y = read_zscored(argv[2], &N);
    check("test_ts", "AC_nl_035", AC_nl_035(y, N), 0.029388510653943489);
    check("test_ts", "AC_nl_036", AC_nl_036(y, N), -0.089862158681074611);
    check("test_ts", "AC_nl_112", AC_nl_112(y, N), 0.95527350930096855);
    check("test_ts", "CO_HistogramAMI_even_2_3", CO_HistogramAMI_even_2_3(y, N), 0.0064026521213012694);
    check("test_ts", "CO_HistogramAMI_even_10_3", CO_HistogramAMI_even_10_3(y, N), 0.069437467855260393);
    check("test_ts", "IN_AutoMutualInfoStats_diff_20_gaussian_ami8",
          IN_AutoMutualInfoStats_diff_20_gaussian_ami8(y, N), 0.0017376086763743674);
    check("test_ts", "DN_RemovePoints_absclose_05_ac2rat",
          DN_RemovePoints_absclose_05_ac2rat(y, N), 0.36055461984180104);
    check("test_ts", "CO_TranslateShape_circle_35_pts_std",
          CO_TranslateShape_circle_35_pts_std(y, N), 0.81476734130849149);
    check("test_ts", "CO_TranslateShape_circle_35_pts_statav4_m",
          CO_TranslateShape_circle_35_pts_statav4_m(y, N), 0.063948212366642623);
    check("test_ts", "SY_DriftingMean50_min", SY_DriftingMean50_min(y, N), -0.17515608004321692);
    check("test_ts", "PH_Walker_biasprop_05_01_sw_meanabsdiff",
          PH_Walker_biasprop_05_01_sw_meanabsdiff(y, N), 0.79368904259375106);
    check("test_ts", "PH_Walker_momentum_5_w_momentumzcross",
          PH_Walker_momentum_5_w_momentumzcross(y, N), 0.15016685205784205);
    check("test_ts", "FC_LoopLocalSimple_mean_stderr_chn",
          FC_LoopLocalSimple_mean_stderr_chn(y, N), 0.090009471873276056);
    check("test_ts", "ST_LocalExtrema_n100_diffmaxabsmin",
          ST_LocalExtrema_n100_diffmaxabsmin(y, N), 0.78721699565949121);
    free(y);

    // --- ST_LocalExtrema for N not a multiple of 100: hctsa's buffer(y,wl)
    // uses ceil(N/wl) windows, not 100 ---
    y = read_zscored(join(dir, "wn299.txt"), &N);
    check("wn299", "ST_LocalExtrema_n100_diffmaxabsmin",
          ST_LocalExtrema_n100_diffmaxabsmin(y, N), 0.91471424058108486);
    free(y);
    y = read_zscored(join(dir, "rw437.txt"), &N);
    check("rw437", "ST_LocalExtrema_n100_diffmaxabsmin",
          ST_LocalExtrema_n100_diffmaxabsmin(y, N), 0.97902825419057138);
    free(y);

    // --- DN_RemovePoints with tied |y|: MATLAB's sort is stable ---
    y = read_zscored(join(dir, "int1000.txt"), &N);
    check("int1000", "DN_RemovePoints_absclose_05_ac2rat",
          DN_RemovePoints_absclose_05_ac2rat(y, N), 0.21171499248284334);
    free(y);

    // --- SC_FluctAnal: fewer than 8 points in r2 -> NaN (as in hctsa) ---
    y = read_zscored(join(dir, "wn150.txt"), &N);
    check("wn150", "SC_FluctAnal_2_dfa_50_2_logi_r2_se2",
          SC_FluctAnal_2_dfa_50_2_logi_r2_se2(y, N), NAN);
    free(y);

    // --- Short series: NaN rather than a number or a crash ---
    double x[30], x_z[30];
    for (int i = 0; i < 30; i++)
        x[i] = sin(0.7 * i * i) + 0.01 * i;
    zscore_norm2(x, 30, x_z);
    // fewer than 12 timescales: hctsa gives NaN (was 0)
    check("short30", "SC_FluctAnal_2_dfa_50_2_logi_r2_se2",
          SC_FluctAnal_2_dfa_50_2_logi_r2_se2(x_z, 30), NAN);
    // training length longer than the series (was a segmentation fault;
    // keep this check last)
    zscore_norm2(x, 8, x_z);
    check("short8", "FC_LoopLocalSimple_mean_stderr_chn",
          FC_LoopLocalSimple_mean_stderr_chn(x_z, 8), NAN);

    printf("%d passed, %d failed\n", n_pass, n_fail);
    return n_fail > 0;
}
