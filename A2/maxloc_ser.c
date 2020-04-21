#include <stdio.h>
#include <stdlib.h>
#include <time.h> 
#include <sys/time.h>
#include <math.h>  

double mysecond();

int main(int argc, char *argv[]) {
    
    int N = 1e6; //size of array
    double x[N]; 
    srand(time(0)); // seed
    int trials = 100; //number of trals
    double t1, t2; // timers
    double time[trials]; 
    double avg = 0; 


    for (int j = 0; j<trials; j++){
        // initialize x array
        for(int i=0; i < N;i++){
            // Generate random number between 0 and 1000
            x[i] = ((double)(rand()) / RAND_MAX)*((double)(rand()) / RAND_MAX)*((double)(rand()) / RAND_MAX)*1000;
        }
        t1 = mysecond();
        // find maxloc
        double maxval = 0.0; 
        int maxloc = 0;
        for (int i=0; i < N; i++){
            if (x[i] > maxval) {
                maxval = x[i]; 
                maxloc = i;
            }
        }
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

// function with timer                                                             
double mysecond(){
    struct timeval tp;
    struct timezone tzp;
    int i;

    i = gettimeofday(&tp,&tzp);
    return ( (double) tp.tv_sec + (double) tp.tv_usec * 1.e-6 );
}