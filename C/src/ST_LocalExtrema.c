#include "ST_LocalExtrema.h"

double ST_LocalExtrema_n100_diffmaxabsmin(const double y[], const int size) {
    
    int wl = floor(size/100); // window length, for (about) 100 windows
    if (wl > size  || wl <= 1) {
        printf("Error in ST_LocalExtrema: window length less than 1 or greater than ts \n");
        fprintf(stderr, "Couldn't divide the time series into multiple windows...\n");
        return NAN;
    }

    // Dividing the ts into non-overlapping windows of length wl. As in hctsa
    // (buffer(y,wl), then removing the zero-padded last window), this gives
    // floor(size/wl) windows, which is more than 100 when size is not a
    // multiple of 100

    int num_windows = size/wl;
    double (*y_buff)[wl] = malloc(sizeof(double[num_windows][wl]));
    int i, j;
    for (i = 0; i < num_windows; i++) {
        for (j = 0; j < wl; j++) {
            if (i*wl+j < size)
                y_buff[i][j] = y[i*wl+j];
            else
                y_buff[i][j] = 0;
        }
    }

    // If last window is all zero then remove it

    int all_zero = 1;
    for (i = 0; i < wl; i++) {
        if (y_buff[num_windows-1][i] != 0)
            all_zero = 0;
    }
    if (all_zero)
        num_windows--;

    // Find Local Extrema

    double locmax, abslocmin, diffmaxabsmin = 0;
    for (i = 0; i < num_windows ; i++) {
        locmax = max_(y_buff[i], wl);
        abslocmin = fabs(min_(y_buff[i], wl));
        diffmaxabsmin += fabs(locmax - abslocmin);
    }
    diffmaxabsmin = diffmaxabsmin/num_windows;

    free(y_buff);
    
    return diffmaxabsmin;
}
