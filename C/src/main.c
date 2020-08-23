#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "CO_AutoCorr.h"
#include "CO_NonlinearAutocorr.h"
#include "SY_DriftingMean.h"
#include "ST_LocalExtrema.h"
#include "DN_RemovePoints.h"
#include "IN_AutoMutualInfoStats.h"
#include "CO_AddNoise.h"
#include "PH_Walker.h"
#include "CO_TranslateShape.h"
#include "SY_SlidingWindow.h"
#include "SC_FluctAnal.h"
#include "FC_LoopLocalSimple.h"

#include "CO_HistogramAMI.h"
#include "stats.h"
#include "helper_functions.h"

void print_help(char *argv[], char msg[])
{
    if (strlen(msg) > 0) {
        fprintf(stdout, "ERROR: %s\n", msg);
    }
    fprintf(stdout, "Usage is %s <infile> <outfile>\n", argv[0]);
    fprintf(stdout, "\n\tSpecifying outfile is optional, by default it is stdout\n");
    exit(1);
}

void run_features(double y[], int size, FILE * outfile)
{   
    double * y_zscored = malloc(size * sizeof * y_zscored);
    
    // variables to keep time
    clock_t begin;
    double timeTaken;
    
    // output
    double result;
    
    // z-scored the time series
    zscore_norm2(y, size, y_zscored);
    
    begin = clock();
    result = AC_nl_035(y_zscored, size);
    timeTaken = (double)(clock()-begin)*1000/CLOCKS_PER_SEC;
    fprintf(outfile, "%.14f, %s, %f\n", result, "AC_nl_035", timeTaken);
    
    begin = clock();
    result = AC_nl_036(y_zscored, size);
    timeTaken = (double)(clock()-begin)*1000/CLOCKS_PER_SEC;
    fprintf(outfile, "%.14f, %s, %f\n", result, "AC_nl_036", timeTaken);
    
    begin = clock();
    result = AC_nl_112(y_zscored, size);
    timeTaken = (double)(clock()-begin)*1000/CLOCKS_PER_SEC;
    fprintf(outfile, "%.14f, %s, %f\n", result, "AC_nl_112", timeTaken);
    
    begin = clock();
    result = CO_HistogramAMI_even_2_3(y_zscored, size);
    timeTaken = (double)(clock()-begin)*1000/CLOCKS_PER_SEC;
    fprintf(outfile, "%.14f, %s, %f\n", result, "CO_HistogramAMI_even_2_3", timeTaken);
    
    begin = clock();
    result = CO_HistogramAMI_even_10_3(y_zscored, size);
    timeTaken = (double)(clock()-begin)*1000/CLOCKS_PER_SEC;
    fprintf(outfile, "%.14f, %s, %f\n", result, "CO_HistogramAMI_even_10_3", timeTaken);
    
    begin = clock();
    result = IN_AutoMutualInfoStats_diff_20_gaussian_ami8(y_zscored, size);
    timeTaken = (double)(clock()-begin)*1000/CLOCKS_PER_SEC;
    fprintf(outfile, "%.14f, %s, %f\n", result, "IN_AutoMutualInfoStats_diff_20_gaussian_ami8", timeTaken);
    
    begin = clock();
    result = CO_AddNoise_1_even_10_ami_at_10(y_zscored, size);
    timeTaken = (double)(clock()-begin)*1000/CLOCKS_PER_SEC;
    fprintf(outfile, "%.14f, %s, %f\n", result, "CO_AddNoise_1_even_10_ami_at_10", timeTaken);
    
    begin = clock();
    result = DN_RemovePoints_absclose_05_ac2rat(y_zscored, size);
    timeTaken = (double)(clock()-begin)*1000/CLOCKS_PER_SEC;
    fprintf(outfile, "%.14f, %s, %f\n", result, "DN_RemovePoints_absclose_05_ac2rat", timeTaken);
    
    begin = clock();
    result = CO_TranslateShape_circle_35_pts_std(y_zscored, size);
    timeTaken = (double)(clock()-begin)*1000/CLOCKS_PER_SEC;
    fprintf(outfile, "%.14f, %s, %f\n", result, "CO_TranslateShape_circle_35_pts_std", timeTaken);
    
    begin = clock();
    result = CO_TranslateShape_circle_35_pts_statav4_m(y_zscored, size);
    timeTaken = (double)(clock()-begin)*1000/CLOCKS_PER_SEC;
    fprintf(outfile, "%.14f, %s, %f\n", result, "CO_TranslateShape_circle_35_pts_statav4_m", timeTaken);
    
    begin = clock();
    result = SY_DriftingMean50_min(y_zscored, size);
    timeTaken = (double)(clock()-begin)*1000/CLOCKS_PER_SEC;
    fprintf(outfile, "%.14f, %s, %f\n", result, "SY_DriftingMean50_min", timeTaken);
    
    begin = clock();
    result = PH_Walker_biasprop_05_01_sw_meanabsdiff(y_zscored, size);
    timeTaken = (double)(clock()-begin)*1000/CLOCKS_PER_SEC;
    fprintf(outfile, "%.14f, %s, %f\n", result, "PH_Walker_biasprop_05_01_sw_meanabsdiff", timeTaken);
    
    begin = clock();
    result = PH_Walker_momentum_5_w_momentumzcross(y_zscored, size);
    timeTaken = (double)(clock()-begin)*1000/CLOCKS_PER_SEC;
    fprintf(outfile, "%.14f, %s, %f\n", result, "PH_Walker_momentum_5_w_momentumzcross", timeTaken);
    
    begin = clock();
    result = FC_LoopLocalSimple_mean_stderr_chn(y_zscored, size);
    timeTaken = (double)(clock()-begin)*1000/CLOCKS_PER_SEC;
    fprintf(outfile, "%.14f, %s, %f\n", result, "FC_LoopLocalSimple_mean_stderr_chn", timeTaken);
    
    begin = clock();
    result = ST_LocalExtrema_n100_diffmaxabsmin(y_zscored, size);
    timeTaken = (double)(clock()-begin)*1000/CLOCKS_PER_SEC;
    fprintf(outfile, "%.14f, %s, %f\n", result, "ST_LocalExtrema_n100_diffmaxabsmin", timeTaken);
    
    begin = clock();
    result = SC_FluctAnal_2_dfa_50_2_logi_r2_se2(y_zscored, size);
    timeTaken = (double)(clock()-begin)*1000/CLOCKS_PER_SEC;
    fprintf(outfile, "%.14f, %s, %f\n", result, "SC_FluctAnal_2_dfa_50_2_logi_r2_se2", timeTaken);
        
    free(y_zscored);
}

int main(int argc, char * argv[])
{
    
    FILE * infile, * outfile;
    int array_size;
    double * y;
    int size;
    double value;
    
    switch (argc) {
        case 1:
            print_help(argv, "");
            break;
        case 2:
            if ((infile = fopen(argv[1], "r")) == NULL) {
                print_help(argv, "Can't open input file\n");
            }
            outfile = stdout;
            break;
        case 3:
            if ((infile = fopen(argv[1], "r")) == NULL) {
                print_help(argv, "Can't open input file\n");
            }
            if ((outfile = fopen(argv[2], "w")) == NULL) {
                print_help(argv, "Can't open output file\n");
            }
            break;
    }
    
    array_size = 50;
    size = 0;
    y = malloc(array_size * sizeof *y);

    while (fscanf(infile, "%lf", &value) != EOF) {
        if (size == array_size) {
            y = realloc(y, 2 * array_size * sizeof *y);
            array_size *= 2;
        }
        y[size++] = value;
    }
    fclose(infile);
    y = realloc(y, size * sizeof *y);
    run_features(y, size, outfile);
    fclose(outfile);
    free(y);
    
    return 0;
}
