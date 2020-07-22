#ifndef DN_REMOVEPOINTS_H
#define DN_REMOVEPOINTS_H

// struct for storing the sorted array 
// elements with it's original index.
struct array {
    int ind;
    double data;
};

int compare_array (const void * a, const void * b);
int compare_int (const void * a, const void * b);
void abs_array(const double y[], int size, struct array *abs_y);

extern double* SUB_acf(const double y[], const int size, double acf[], int lag);
extern double DN_RemovePoints_absclose_05_ac2rat(const double y[], const int size);

#endif
