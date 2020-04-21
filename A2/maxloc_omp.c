#include <stdio.h>
#include <stdlib.h>
#include <time.h> 
#include <sys/time.h>
#include <math.h>  
#include <omp.h>

double mysecond();

int main(int argc, char *argv[]) {
    
    int N = 1e6; //size of array
    double x[N]; 
    srand(time(0)); // seed
    int trials = 100; //number of trals
    double t1, t2; // timers
    double time[trials]; 
    double avg = 0; 

    int threads[] = {1,2,4,8,16,20,24,28,32};
    typedef struct {double val; int loc; char pad[128]; } tvals;

    omp_set_dynamic(0);

    for (int k = 0; k<9; k++){

        omp_set_num_threads(threads[k]);
        int MAX_THREADS = omp_get_max_threads();
        printf("Threads: %d\n", threads[k]);

        for (int j = 0; j<trials; j++){
            // initialize x array
            for(int i=0; i < N;i++){
                // Generate random number between 0 and 1000
                x[i] = ((double)(rand()) / RAND_MAX)*((double)(rand()) / RAND_MAX)*((double)(rand()) / RAND_MAX)*1000;
            }
            t1 = mysecond();

            /* ---- Uncomment correct section of code below! ---- */

            //// maxloc parallel for
            // double maxval = 0.0; 
            // int maxloc = 0;
            // #pragma omp parallel for
            // for (int i=0; i < N; i++){
            //     if (x[i] > maxval) {
            //         maxval = x[i]; 
            //         maxloc = i;
            //     }
            // } 

            //// maxloc parallel for critical
            // double maxval = 0.0; 
            // int maxloc = 0;
            // #pragma omp parallel for
            // for (int i=0; i < N; i++){
            //     #pragma omp critical
            //     {
            //         if (x[i] > maxval) {
            //             maxval = x[i]; 
            //             maxloc = i;
            //         }
            //     }
            // }

            ////maxloc parallel for with temporary arrays
            // double maxval, mval[MAX_THREADS]; 
            // int maxloc, mloc[MAX_THREADS];
            // #pragma omp parallel shared(mval,mloc)
            // {
            //     int id = omp_get_thread_num();
            //     mval[id] = 0.0;
            //     #pragma omp for
            //     for (int i=0; i < N; i++){
            //         if (x[i] > mval[id]) {
            //             mval[id] = x[i]; 
            //             mloc[id] = i;
            //         }
            //     }
            // }
            // maxval = mval[0];
            // maxloc = mloc[0];
            // for (int i; i<MAX_THREADS; i++){
            //     if (mval[i] > maxval){
            //         maxval = mval[i];
            //         maxloc = mloc[i];
            //     }
            // }

            ////maxloc parallel for with temporary arrays and padding
            // double maxval; 
            // int maxloc;
            // tvals maxinfo[MAX_THREADS];
            // #pragma omp parallel shared(maxinfo)
            // {
            //     int id = omp_get_thread_num();
            //     maxinfo[id].val = -10;
            //     #pragma omp for
            //     for (int i=0; i < N; i++){
            //         if (x[i] > maxinfo[id].val) {
            //             maxinfo[id].val = x[i]; 
            //             maxinfo[id].loc = i;
            //         }
            //     }
            // }
            // maxval = maxinfo[0].val;
            // maxloc = maxinfo[0].loc;
            // for (int i; i<MAX_THREADS; i++){
            //     if (maxinfo[i].val > maxval){
            //         maxval = maxinfo[i].val;
            //         maxloc = maxinfo[i].loc;
            //     }
            // }

            t2 = mysecond();
            printf("maximum is %f at position %d\n", maxval,maxloc);
            time[j] = t2-t1;
            avg += (t2-t1);
        }
        avg = avg/trials;

        //find standard deviation
        double std_dev = 0;
        for (int j = 0; j<trials; j++){
            std_dev += pow(time[j]-avg, 2);
        }
        std_dev = sqrt(std_dev/trials);

        printf("average execution time : %f\nstandard deviation: %f\n", avg,std_dev);
    }
}

// function with timer                                                             
double mysecond(){
    struct timeval tp;
    struct timezone tzp;
    int i;

    i = gettimeofday(&tp,&tzp);
    return ( (double) tp.tv_sec + (double) tp.tv_usec * 1.e-6 );
}