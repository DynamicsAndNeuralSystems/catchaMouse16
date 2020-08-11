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

char* concat_str(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

int main()
{
    
    //printf("%*s\n", strcat(strcat("Input/","Left_CAMK_excitatory"),"/hctsa_timeseries-data.csv"));

    char *feature_list[] = {"SY_DriftingMean50_min","DN_RemovePoints_absclose_05_remove_ac2rat","AC_nl_036","AC_nl_112","ST_LocalExtrema_n100_diffmaxabsmin",
                            "IN_AutoMutualInfoStats_diff_20_gaussian_ami8","CO_HistogramAMI_even_2_3","CO_TranslateShape_circle_35_pts_statav4_m",
                            "CO_AddNoise_1_even_10_ami_at_10","PH_Walker_momentum_5_w_momentumzcross","SC_FluctAnal_2_dfa_50_2_logi_r2_se2",
                            "PH_Walker_biasprop_05_01_sw_meanabsdiff","CO_HistogramAMI_even_10_3","AC_nl_035","FC_LoopLocalSimple_mean_stderr_chn",
                            "CO_TranslateShape_circle_35_pts_std"};
    char *taskNames[] = {"Left_CAMK_excitatory","Left_CAMK_PVCre","Left_CAMK_SHAM","Left_excitatory_PVCre","Left_excitatory_SHAM","Left_PVCre_SHAM","Right_CAMK_excitatory","Right_CAMK_PVCre","Right_CAMK_SHAM","Right_excitatory_PVCre","Right_excitatory_SHAM","Right_PVCre_SHAM"};
    int total_feat = 16;
    //int total_task = 12;
    //char *taskNames = "Left_CAMK_excitatory";
    char *folder = concat_str("Input/",taskNames[0]);
    FILE *fp1 = fopen(concat_str(folder,"/hctsa_timeseries-data.csv"), "r");
    FILE *fp3 = fopen(concat_str(folder,"/hctsa_datamatrix.csv"), "r");
    FILE *fp2 = fopen(concat_str(folder,"/hctsa_features.csv"), "r");
    FILE *fp4 = fopen(concat_str(folder,"/output_feature_val.csv"), "w");
    FILE *fp5 = fopen(concat_str(folder,"/output_feature_time.csv"), "w");
    if (!fp1 || !fp2 || !fp3)
    {
        printf("Can't open the input CSV files");
        return 0;
    }
    // check the id of feature in hctsa_features.csv
    int *feature_ids = (int*) malloc(total_feat*sizeof(int));

    /*feature_ids[0] = 2225;
    feature_ids[1] = 1704;
    feature_ids[2] = 184;
    feature_ids[3] = 187;
    feature_ids[4] = 3475;
    feature_ids[5] = 386;
    feature_ids[6] = 241;
    feature_ids[7] = 1885;
    feature_ids[8] = 1192;
    feature_ids[9] = 2726;
    feature_ids[10] = 4181;
    feature_ids[11] = 2667;
    feature_ids[12] = 251;
    feature_ids[13] = 180;
    feature_ids[14] = 3098;
    feature_ids[15] = 1870;*/
    char feat_line[1024];
    int first = 1;
    while (fgets(feat_line, 1024, fp2)) {
        if (first) {
            first = 0;
            continue;
        }
        char *subfield = strtok(feat_line, ","); // problem --
        int field_no = 0;
        int fid;
        while (subfield) {
            //printf("%s\n", subfield);
            if (field_no==0) {
                fid = atoi(subfield);
            }
            else if (field_no==2) {
                for(int i = 0; i < total_feat; i++) {
                    if ( !strcmp(subfield, feature_list[i]) )
                        feature_ids[i] = fid;
                }
            }
            //else
            //    break;
            subfield = strtok(NULL, ",");
            field_no++;
            //break;
        }
        //break;
    }
    /*for(int i = 0; i < total_feat; i++) {
        printf("%d\t", feature_ids[i]);
    }*/

    //exit(0);
    clock_t begin;

    double *feature_val = (double*) malloc(total_feat * sizeof(double));
    double *feature_time = (double*) malloc(total_feat * sizeof(double));
    char line[10000*12]; // line -- length of timeseries is 10000
    int row_count = 0;
    int field_count = 0;
    //int *ts_len = (double*) malloc(1000 * sizeof(int));


    while (fgets(line, 10000*12, fp1)) {
        field_count = 0;
        row_count++;
        printf("Reading the time series..\n");
        double *ts = (double*) malloc(10000*sizeof(double));
        double *ts_norm = (double*) malloc(10000*sizeof(double));
        int ts_ind = 0;
        char *field = strtok(line, ",");
        while (field) {
            ts[ts_ind++] = strtod(field, NULL);
            //printf("%s\n", field);
            field = strtok(NULL, ",");
            field_count++;
        }
        zscore_norm2(ts, ts_ind, ts_norm);
        //printf("ts[0] = %lf\n", ts[]);

        // compute my version of feature on TS
        
        // ... for all features ...
        printf("Computing the feature values..\n");
        
        begin = clock();
        feature_val[0] = SY_DriftingMean50_min(ts_norm, ts_ind);
        feature_time[0] = (double)(clock()-begin)*1000/CLOCKS_PER_SEC;

        begin = clock();
        feature_val[1] = DN_RemovePoints_absclose_05_ac2rat(ts_norm, ts_ind);
        feature_time[1] = (double)(clock()-begin)*1000/CLOCKS_PER_SEC;

        begin = clock();
        feature_val[2] = AC_nl_036(ts_norm, ts_ind);
        feature_time[2] = (double)(clock()-begin)*1000/CLOCKS_PER_SEC;

        begin = clock();
        feature_val[3] = AC_nl_112(ts_norm, ts_ind);
        feature_time[3] = (double)(clock()-begin)*1000/CLOCKS_PER_SEC;

        begin = clock();
        feature_val[4] = ST_LocalExtrema_n100_diffmaxabsmin(ts_norm, ts_ind);
        feature_time[4] = (double)(clock()-begin)*1000/CLOCKS_PER_SEC;

        begin = clock();
        feature_val[5] = IN_AutoMutualInfoStats_diff_20_gaussian_ami8(ts_norm, ts_ind);
        feature_time[5] = (double)(clock()-begin)*1000/CLOCKS_PER_SEC;

        begin = clock();
        feature_val[6] = CO_HistogramAMI_even_2_3(ts_norm, ts_ind);
        feature_time[6] = (double)(clock()-begin)*1000/CLOCKS_PER_SEC;

        begin = clock();
        feature_val[7] = CO_TranslateShape_circle_35_pts_statav4_m(ts_norm, ts_ind);
        feature_time[7] = (double)(clock()-begin)*1000/CLOCKS_PER_SEC;
        // CO_AddNoise -- id = 1192
        
        begin = clock();
        feature_val[8] = CO_AddNoise_1_even_10_ami_at_10(ts_norm, ts_ind);
        feature_time[8] = (double)(clock()-begin)*1000/CLOCKS_PER_SEC;

        begin = clock();
        feature_val[9] = PH_Walker_momentum_5_w_momentumzcross(ts_norm, ts_ind);
        feature_time[9] = (double)(clock()-begin)*1000/CLOCKS_PER_SEC;

        begin = clock();
        feature_val[10] = SC_FluctAnal_2_dfa_50_2_logi_r2_se2(ts_norm, ts_ind);
        feature_time[10] = (double)(clock()-begin)*1000/CLOCKS_PER_SEC;

        begin = clock();
        feature_val[11] = PH_Walker_biasprop_05_01_sw_meanabsdiff(ts_norm, ts_ind);
        feature_time[11] = (double)(clock()-begin)*1000/CLOCKS_PER_SEC;

        begin = clock();
        feature_val[12] = CO_HistogramAMI_even_10_3(ts_norm, ts_ind);
        feature_time[12] = (double)(clock()-begin)*1000/CLOCKS_PER_SEC;

        begin = clock();
        feature_val[13] = AC_nl_035(ts_norm, ts_ind);
        feature_time[13] = (double)(clock()-begin)*1000/CLOCKS_PER_SEC;

        begin = clock();
        feature_val[14] = FC_LoopLocalSimple_mean_stderr_chn(ts_norm, ts_ind);
        feature_time[14] = (double)(clock()-begin)*1000/CLOCKS_PER_SEC;

        begin = clock();
        feature_val[15] = CO_TranslateShape_circle_35_pts_std(ts_norm, ts_ind);
        feature_time[15] = (double)(clock()-begin)*1000/CLOCKS_PER_SEC;

        for (int i = 0; i < total_feat; i++) {
            fprintf(fp4, "%.12lf", feature_val[i]);
            fprintf(fp5, "%lf", feature_time[i]);
            if (i != total_feat - 1) {
                fprintf(fp4, ", ");
                fprintf(fp5, ", ");
            }
        }
        fprintf(fp4, "\n");
        fprintf(fp5, "\n");
        // now compare with the value in datamatrix.csv in that index   
        free(ts_norm);
        free(ts);
        //break; // only compare for the first time series
    }
    //exit(0);

    /*for (int i = 0; i < total_feat; i++)
    {
        printf("id: %d, value: %lf\n", feature_ids[i],feature_val[i]);
    }*/

    printf("Comparing the values with datamatrix..\n");
    char dline[7800*12];
    int count = 1;     
    while (fgets(dline, 7800*12, fp3)) {
        char *matfield = strtok(dline, ",");
        while (matfield) {
            for (int i = 0; i < total_feat; i++) {
                if (count == feature_ids[i])
                    printf("Feature id: %d, name: %s\nExpected value =\t%.14lf\nComputed value =\t%.14lf\nTime Taken =\t\t%lf ms\n", 
                                feature_ids[i], feature_list[i], strtod(matfield,NULL), feature_val[i], feature_time[i]);
            }
            matfield = strtok(NULL, ",");
            count++;
        }
        break; // only compare for first time series
    }

    free(feature_val);
    free(feature_ids);

    fclose(fp1);
    fclose(fp2);
    fclose(fp3);
    fclose(fp4);
    
    return 0;
}
