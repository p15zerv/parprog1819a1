#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>


struct timeval tp;

void allocate(float**, float**, float**, int);
void deallocate(float**, float**, float**);
void get_walltime(double*);

int main() 
{
    float *fa, *fb, *fc;
    allocate(&fa, &fb, &fc, N*N);

    // Array initialization
    for (int i=0; i<N*N; i++) {
        fa[i] = 2.0;
        fb[i] = 3.0;
        fc[i] = 20.0;
    }

    double ts, te;
    float *pa, *pb = fb, *pc = fc;
    float sum;

    // Get start time
    get_walltime(&ts);

    // Workload
    for (int k=0; k<N; k++) {
        pb = fb;
        for (int j=0; j<N; j++) {
            sum = 0;
            pa = fa + k*N;
            for (int i=0; i<N; i++) {
                sum += (*pa) * (*pb);
                pa++;
                pb++;
            }
            *pc = sum;
            pc++;
        }
    }

    // Get end time
    get_walltime(&te);

    // Check if results are correct
    for (int i=0; i<N*N; i++) {
        if (fc[i] != 6 * N) {
            printf("Wrong result!\n");
            break;
        }
    }

    // Print time and mflops (formatting is minimal as the output is redirected to a file to be graphed)
    printf("%lf\t%lf\n", te-ts, (unsigned long)N*N*N / ((te-ts) * 1e6));

    deallocate(&fa, &fb, &fc);
    return 0;
}

void allocate(float** a, float** b, float** c, int n) {
    *a = (float*) malloc(sizeof(float) * n);
    if (!*a) {
        exit(-1);
    }
    *b = (float*) malloc(sizeof(float) * n);
    if (!*b) {
        free(*a);
        exit(-1);
    }
    *c = (float*) malloc(sizeof(float) * n);
    if (!*c) {
        free(*a);
        free(*b);
        exit(-1);
    }
}

void deallocate(float **a, float **b, float **c)
{
    free(*a);
    free(*b);
    free(*c);
}

void get_walltime(double *wct)
{
    struct timeval tp;
    gettimeofday(&tp, NULL);
    *wct = (double) (tp.tv_sec + tp.tv_usec/1000000.0);
}
