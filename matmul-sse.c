#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pmmintrin.h>


struct timeval tp;

void deallocate(float**, float**, float**);
void get_walltime(double*);

int main() 
{
    float *fa, *fb, *fc;
    __m128 *vfa, *vfb;

    // Allocate memory and request arrays are aligned at 16 bytes
    if (posix_memalign((void**) &fa, 16, N * N * sizeof(float)) != 0) {
        printf("Error allocating memory!\n");
        exit(-1);
    }
    if (posix_memalign((void**) &fb, 16, N * N * sizeof(float)) != 0) {
        free(fa);
        printf("Error allocating memory!\n");
        exit(-1);
    }
    if (posix_memalign((void**) &fc, 16, N * N * sizeof(float)) != 0) {
        free(fa);
        free(fb);
        printf("Error allocating memory!\n");
        exit(-1);
    }

    // Initialization
    for (int i=0; i<N*N; i++) {
        fa[i] = 2.0;
        fb[i] = 3.0;
        fc[i] = 20.0;
    }

    double ts, te;
    float *pa, *pb, *pc = fc;
    float *zero;
    if (posix_memalign((void **) &zero, 16, 4 * sizeof(float)) != 0) {
        free(fa);
        free(fb);
        free(fc);
        printf("Error allocating memory!\n");
        exit(-1);
    }
    for (int i=0; i<4; i++) {
        zero[i] = 0.0;
    }
    __m128 *sum = (__m128*) zero;
    __m128 tmp;

    // Get start time
    get_walltime(&ts);

    // Workload
    for (int k=0; k<N; k++) {
        pb = fb;
        vfb = (__m128*) pb;
        for (int j=0; j<N; j++) {
            pa = fa + k*N;
            vfa = (__m128*) pa;

            // reset `sum` to 0
            *sum = _mm_set_ps(0, 0, 0, 0);
            for (int i=0; i<N; i+=4) {
                // `sum` holds the dot product of fa's and fb's current row
                *sum = _mm_add_ps(*sum, _mm_mul_ps(*vfa, *vfb));
                
                vfa++, vfb++;
            }
            // The 4 elements of `sum` need to be added up, this is done using the SSE3 hadd operation
            tmp = _mm_hadd_ps(*sum, *sum);
            *sum = _mm_hadd_ps(tmp, tmp);

            *pc = zero[0];
            pc++;
        }
    }

    // Get end time
    get_walltime(&te);

    // Check if results are correct
    for (int i=0; i<N*N; i++) {
        if (fc[i] != 6.0 * N) {
            printf("Wrong result! Expected %f, but instead found fc[%d]=%f\n", 6.0*N, i, fc[i]);
            break;
        }
    }

    // Print time and mflops (formatting is minimal as the output is redirected to a file to be graphed)
    printf("%lf\t%lf\n", te-ts, (unsigned long)N*N*N / ((te-ts) * 1e6));

    deallocate(&fa, &fb, &fc);
    return 0;
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
