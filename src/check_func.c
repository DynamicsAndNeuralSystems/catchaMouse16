#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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

//#include "SP_Summaries.h"
//#include ""
#include "stats.h"
#include "helper_functions.h"


int main()
{
    char *feature_list[] = {"SY_DriftingMean50_min","DN_RemovePoints_absclose_05_ac2rat","AC_nl_036","AC_nl_112","ST_LocalExtrema_n100_diffmaxabsmin","IN_AutoMutualInfoStats_diff_20_gaussian_ami8","CO_HistogramAMI_even_2_3","CO_TranslateShape_circle_35_pts_statav4_m","CO_AddNoise_1_even_10_ami_at_10","PH_Walker_momentum_5_w_momentumzcross"};
    int total_feat = 10;
    FILE *fp1 = fopen("Input/hctsa_timeseries-data.csv", "r");
    FILE *fp3 = fopen("Input/hctsa_datamatrix.csv", "r");
    FILE *fp2 = fopen("Input/hctsa_features.csv", "r");
    if (!fp1 || !fp2 || !fp3)
    {
        printf("Can't open the input CSV files");
        return 0;
    }

    // check the id of feature in hctsa_features.csv
    int *feature_ids = (int*) malloc(total_feat*sizeof(int));

    feature_ids[0] = 2225;
    feature_ids[1] = 1704;
    feature_ids[2] = 184;
    feature_ids[3] = 187;
    feature_ids[4] = 3475;
    feature_ids[5] = 386;
    feature_ids[6] = 241;
    feature_ids[7] = 1885;
    feature_ids[8] = 1192;
    feature_ids[9] = 2726;
    /*char feat_line[1024];
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
            printf("%s\n", subfield);
            if(field_no==0)
                fid = atoi(subfield);
            if(field_no==3)
            {
                for(int i = 0; i < 6; i++) //compare with 6 features name
                {
                    if ( !strcmp(subfield, feature_list[i]) )
                        feature_ids[i] = fid;
                }
            }
            subfield = strtok(NULL, "\t");
            field_no++;
            //break;
        }
        //break;
    }*/
    

    //exit(0);

    double *feature_val = (double*) malloc(total_feat * sizeof(double));
    char line[10000*12]; // line -- length of timeseries is 10000
    int row_count = 0;
    int field_count = 0;

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
        zscore_norm2(ts, 10000, ts_norm);
        //printf("ts[0] = %lf\n", ts[]);

        // compute my version of feature on TS
        
        // ... for all features ...
        printf("Computing the feature values..\n");
        feature_val[0] = SY_DriftingMean50_min(ts_norm, ts_ind);
        
        feature_val[1] = DN_RemovePoints_absclose_05_ac2rat(ts_norm, ts_ind);
        
        feature_val[2] = AC_nl_036(ts_norm, ts_ind);
        feature_val[3] = AC_nl_112(ts_norm, ts_ind);
        
        feature_val[4] = ST_LocalExtrema_n100_diffmaxabsmin(ts_norm, ts_ind);
        feature_val[5] = IN_AutoMutualInfoStats_diff_20_gaussian_ami8(ts_norm, ts_ind);
        feature_val[6] = CO_HistogramAMI_even_2_3(ts_norm, ts_ind);
        feature_val[7] = CO_TranslateShape_circle_35_pts_statav4_m(ts_norm, ts_ind);
        // CO_AddNoise -- id = 1192
        feature_val[8] = CO_AddNoise_1_even_10_ami_at_10(ts_norm, ts_ind);
        feature_val[9] = PH_Walker_momentum_5_w_momentumzcross(ts_norm, ts_ind);
        // now compare with the value in datamatrix.csv in that index   
        free(ts_norm);
        free(ts);
        break; // only compare for the first time series
    }
    //exit(0);

    for (int i = 0; i < total_feat; i++)
    {
        printf("id: %d, value: %lf\n", feature_ids[i],feature_val[i]);
    }

    printf("Comparing the values with datamatrix..\n");
    char dline[7800*12];
    int count = 1;     
    while (fgets(dline, 7800*12, fp3)) {
        char *matfield = strtok(dline, ",");
        while (matfield) {
            for (int i = 0; i < total_feat; i++)
            {
                if (count == feature_ids[i])
                {
                    printf("Feature id: %d, name: %s\nExpected value =\t%.14lf\nComputed value =\t%.14lf\n", 
                                feature_ids[i], feature_list[i], strtod(matfield,NULL), feature_val[i]);
                }
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
    
    return 0;
}
