#include <stdio.h>
#include <stdlib.h>
#include <time.h> 
#include <sys/time.h>

int main(int argc, char *argv[]) {
    
    int N = 1000; //no of particles
    srand(time(0)); //seed

    //genenerate particles
    double pos[N][3];
    double old_pos[N][3];
    double vel[N][3];
    double mass[N];
    for  (int q=0; q<N; q++){
        pos[q][0] = (rand() / (double)(RAND_MAX)) * 2 - 1;
        pos[q][1] = (rand() / (double)(RAND_MAX)) * 2 - 1;
        pos[q][2] = (rand() / (double)(RAND_MAX)) * 2 - 1;

        old_pos[q][0] = pos[q][0];
        old_pos[q][1] = pos[q][1];
        old_pos[q][2] = pos[q][2];

        vel[q][X] = (rand() / (double)(RAND_MAX)) * 2 - 1;
		vel[q][Y] = (rand() / (double)(RAND_MAX)) * 2 - 1;
		vel[q][Z] = (rand() / (double)(RAND_MAX)) * 2 - 1;

		mass[q] = fabs((rand() / (double)(RAND_MAX)) * 2 - 1);
    }

}