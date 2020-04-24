#include <stdio.h>
#include <stdlib.h>
#include <time.h> 
#include <sys/time.h>
#include <math.h>
#include <string.h>

double mysecond();

int main(int argc, char *argv[]) {
#define N 1000
#define X 0
#define Y 1

	srand(1); //seed
	double delta_t = 0.05;
	double fin_t = 2;
	int cycles = 100;//(int)(fin_t/delta_t);
	int freq = 10;
	double G = 6.673e-11;
	double t1, t2; //timers
	int trials = 10; //number of trials
	double time[trials];
	double avg = 0;

	for (int j = 0; j<trials; j++) {
		double x_diff, y_diff, dist, dist_cubed; //variable for calculations

												 //genenerate particles
		double pos[N][2];
		double old_pos[N][2];
		double vel[N][2];
		double forces[N][2];
		double mass[N];
		for (int q = 0; q<N; q++) {
			pos[q][X] = (rand() / (double)(RAND_MAX)) * 2 - 1;
			pos[q][Y] = (rand() / (double)(RAND_MAX)) * 2 - 1;

			old_pos[q][X] = pos[q][X];
			old_pos[q][Y] = pos[q][Y];

			vel[q][X] = (rand() / (double)(RAND_MAX)) * 2 - 1;
			vel[q][Y] = (rand() / (double)(RAND_MAX)) * 2 - 1;

			mass[q] = fabs((rand() / (double)(RAND_MAX)));
		}

		t1 = mysecond();

		//simple algorithm
		for (int t = 0; t<cycles; t++) {
			// if (t % (cycles/freq) == 0){
			//     //print results
			//     printf("results (t = %f):\n", (t*delta_t));
			//     for (int q =0; q<N;q++){
			//         printf("%d position: (%f,%f), velocity: (%f,%f)\n", q, pos[q][X],pos[q][Y],vel[q][X],vel[q][Y]);
			//     }
			// }
			for (int q = 0; q<N; q++) {
				for (int k = 0; k<N; k++) {
					if (k != q) {
						//calculate forces 
						//based on positions at previous timestep
						x_diff = old_pos[q][X] - old_pos[k][X];
						y_diff = old_pos[q][Y] - old_pos[k][Y];
						dist = sqrt(x_diff*x_diff + y_diff*y_diff);
						dist_cubed = dist*dist*dist;
						forces[q][X] -= G*mass[q] * mass[k] / dist_cubed * x_diff;
						forces[q][Y] -= G*mass[q] * mass[k] / dist_cubed * y_diff;
					}
				}
				//move particles
				pos[q][X] += delta_t*vel[q][X];
				pos[q][Y] += delta_t*vel[q][Y];
				vel[q][X] += delta_t / mass[q] * forces[q][X];
				vel[q][Y] += delta_t / mass[q] * forces[q][Y];
			}
			for (int q = 0; q<N; q++) {
				//update positions used for calculating next timestep
				old_pos[q][X] = pos[q][X];
				old_pos[q][Y] = pos[q][Y];
			}
		}

		t2 = mysecond();

		//print results
		//printf("results:\n");
		//for (int q =0; q<N;q++){
		//    printf("%d position: (%f,%f), velocity: (%f,%f)\n", q, pos[q][X],pos[q][Y],vel[q][X],vel[q][Y]);
		//}

		time[j] = t2 - t1;
		avg += (t2 - t1);
	}
	avg = avg / trials;

	//find standard deviation
	double std_dev = 0;
	for (int j = 0; j<trials; j++) {
		std_dev += pow(time[j] - avg, 2);
	}
	std_dev = sqrt(std_dev / trials);

	printf("average execution time : %f\nstandard deviation: %f\n", avg, std_dev);
}

// function with timer                                                             
double mysecond() {
	struct timeval tp;
	struct timezone tzp;
	int i;

	i = gettimeofday(&tp, &tzp);
	return ((double)tp.tv_sec + (double)tp.tv_usec * 1.e-6);
}