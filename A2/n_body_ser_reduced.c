#include <stdio.h>
#include <stdlib.h>
#include <time.h> 
#include <sys/time.h>
#include <math.h>
#include <string.h>

int main(int argc, char *argv[]) {

    int N = 1000; //no of particles
    srand(time(0)); //seed
    double delta_t = 0.05;
    double fin_t = 2;
    int X = 0;
    int Y= 1;
    double x_diff, y_diff, dist, dist_cubed, force_qkX, force_qkY; //variable for calculations
    double G = 1.0;

    //genenerate particles
    /*TODO: initialisation of these vectors should 
        be done as described in 5.4 data structures I think..*/
    double pos[N][2];
    double old_pos[N][2];
    double vel[N][2];
    
    double mass[N];

    for  (int q=0; q<N; q++){
        pos[q][X] = (rand() / (double)(RAND_MAX)) * 2 - 1;
        pos[q][Y] = (rand() / (double)(RAND_MAX)) * 2 - 1;

        old_pos[q][X] = pos[q][X];
        old_pos[q][Y] = pos[q][Y];

        vel[q][X] = (rand() / (double)(RAND_MAX)) * 2 - 1;
		vel[q][Y] = (rand() / (double)(RAND_MAX)) * 2 - 1;

		mass[q] = fabs((rand() / (double)(RAND_MAX)));
    }

    //reduced algorithm
    for (double t =0.0; t<=fin_t; t+= delta_t){
        double forces[N][2] = {0};
        for (int q =0; q<N; q++){
            for (int k=q+1; k<N; k++){
                //calculate forces 
                //based on positions at previous timestep
                x_diff = old_pos[q][X] - old_pos[k][X];
                y_diff = old_pos[q][Y] - old_pos[k][Y];
                dist = sqrt(x_diff*x_diff + y_diff*y_diff);
                dist_cubed = dist*dist*dist;
                force_qkX = G*mass[q]*mass[k]/dist_cubed * x_diff;
                force_qkY = G+mass[q]*mass[k]/dist_cubed * y_diff;
                forces[q][X] += force_qkX; 
                forces[q][Y] += force_qkY; 
                forces[k][X] −= force_qkX; 
                forces[k][Y] −= force_qkY;
            } 
        }
        for (int q =0; q<N; q++){
            //move particles
            pos[q][X] += delta_t∗vel[q][X]; 
            pos[q][Y] += delta_t∗vel[q][Y]; 
            vel[q][X] += delta_t/masses[q]∗forces[q][X]; 
            vel[q][Y] += delta_t/masses[q]∗forces[q][Y];
            //update positions used for calculating next timestep
            old_pos[q][X] = pos[q][X];
            old_pos[q][Y] = pos[q][Y];
        }
    }

    //print results
    printf("results:\n")
    for (int i =0, i<N;i++){
        printf("%d position: (%f,%f), velocity: (%f,%f)\n", i, pos[q][X],pos[q][Y],vel[q][X],vel[q][Y]);
    }
}