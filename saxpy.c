/* Implementation of the SAXPY loop.
 *
 * Compile as follows: gcc -o saxpy saxpy.c -O3 -Wall -std=c99 -lpthread -lm
 *
 * Author: Naga Kandasamy
 * Date created: April 14, 2020
 * Date modified: April 13, 2023 
 *
 * Student names: Baran Arig, Pranav Reddy
 * Date: 4/25
 *
 * */

#define _REENTRANT /* Make sure library functions are MT (muti-thread) safe */
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <pthread.h>

/* Function prototypes */
void compute_gold(float *, float *, float, int);
void compute_using_pthreads_v1(float *, float *, float, int, int);
void compute_using_pthreads_v2(float *, float *, float, int, int);
int check_results(float *, float *, int, float);
void helper_v1(void* ts1_void);
void helper_v2(void* ts2_void);


typedef struct myThreadStruct_V1{
    int num_elements;
    int chunkSize;
    int tid;
    float a;
    float *x;
    float *y;
} myThreadStruct_V1;

typedef struct myThreadStruct_V2{
    int num_elements;
    int chunkSize;
    int tid;
    float a;
    float *x;
    float *y;
    int stride;
} myThreadStruct_V2;


int main(int argc, char **argv)
{
	if (argc < 3) {
		fprintf(stderr, "Usage: %s num-elements num-threads\n", argv[0]);
        fprintf(stderr, "num-elements: Number of elements in the input vectors\n");
        fprintf(stderr, "num-threads: Number of threads\n");
		exit(EXIT_FAILURE);
	}
	
    int num_elements = atoi(argv[1]); 
    int num_threads = atoi(argv[2]);

	/* Create vectors X and Y and fill them with random numbers between [-.5, .5] */
    fprintf(stderr, "Generating input vectors\n");
    int i;
	float *x = (float *)malloc(sizeof(float) * num_elements);
    float *y1 = (float *)malloc(sizeof(float) * num_elements);              /* For the reference version */
	float *y2 = (float *)malloc(sizeof(float) * num_elements);              /* For pthreads version 1 */
	float *y3 = (float *)malloc(sizeof(float) * num_elements);              /* For pthreads version 2 */

	srand(time(NULL)); /* Seed random number generator */
	for (i = 0; i < num_elements; i++) {
		x[i] = rand()/(float)RAND_MAX - 0.5;
		y1[i] = rand()/(float)RAND_MAX - 0.5;
        y2[i] = y1[i]; /* Make copies of y1 for y2 and y3 */
        y3[i] = y1[i]; 
	}

    float a = 2.5;  /* Choose some scalar value for a */

	/* Calculate SAXPY using the reference solution. The resulting values are placed in y1 */
    fprintf(stderr, "\nCalculating SAXPY using reference solution\n");
	struct timeval start, stop;	
	gettimeofday(&start, NULL);
	
    compute_gold(x, y1, a, num_elements); 
	
    gettimeofday(&stop, NULL);
	fprintf(stderr, "Execution time = %fs\n", (float)(stop.tv_sec - start.tv_sec\
                + (stop.tv_usec - start.tv_usec)/(float)1000000));

	/* Compute SAXPY using pthreads, version 1. Results must be placed in y2 */
    fprintf(stderr, "\nCalculating SAXPY using pthreads, version 1\n");
	gettimeofday(&start, NULL);

	compute_using_pthreads_v1(x, y2, a, num_elements, num_threads);
	
    gettimeofday(&stop, NULL);
	fprintf(stderr, "Execution time = %fs\n", (float)(stop.tv_sec - start.tv_sec\
                + (stop.tv_usec - start.tv_usec)/(float)1000000));

    /* Compute SAXPY using pthreads, version 2. Results must be placed in y3 */
    fprintf(stderr, "\nCalculating SAXPY using pthreads, version 2\n");
	gettimeofday(&start, NULL);

	compute_using_pthreads_v2(x, y3, a, num_elements, num_threads);
	
    gettimeofday(&stop, NULL);
	fprintf(stderr, "Execution time = %fs\n", (float)(stop.tv_sec - start.tv_sec\
                + (stop.tv_usec - start.tv_usec)/(float)1000000));

    /* Check results for correctness */
    fprintf(stderr, "\nChecking results for correctness\n");
    float eps = 1e-12;                                      /* Do not change this value */
    if (check_results(y1, y2, num_elements, eps) == 0)
        fprintf(stderr, "TEST PASSED\n");
    else 
        fprintf(stderr, "TEST FAILED\n");
 
    if (check_results(y1, y3, num_elements, eps) == 0)
        fprintf(stderr, "TEST PASSED\n");
    else 
        fprintf(stderr, "TEST FAILED\n");

	/* Free memory */ 
	free((void *)x);
	free((void *)y1);
    free((void *)y2);
	free((void *)y3);

    exit(EXIT_SUCCESS);
}

/* Compute reference soution using a single thread */
void compute_gold(float *x, float *y, float a, int num_elements)
{
	int i;
    for (i = 0; i < num_elements; i++) {
        y[i] = a * x[i] + y[i]; 
    }
    
}

/* Calculate SAXPY using pthreads, version 1. Place result in the Y vector */
void compute_using_pthreads_v1(float *x, float *y, float a, int num_elements, int num_threads)
{
   
pthread_t* threads = (pthread_t *)malloc( num_threads * sizeof(pthread_t));
//pthread_t* threads[num_threads];
myThreadStruct_V1* ts1;

for(int i=0; i < num_threads; i++){
    ts1 = (myThreadStruct_V1*) malloc(sizeof(myThreadStruct_V1));
    ts1->tid = i;
    ts1->x = x;
    ts1->y = y;
    ts1->a = a;
    
    ts1->chunkSize = floor(num_elements/num_threads);

    pthread_create(&threads[i], NULL, (void*)helper_v1, ts1);


}

for(int i=0; i < num_threads; i++){
    pthread_join( threads[i], NULL );
}
free(threads);
/* 
1. Allocate space for pthreads threads[num_threads]
2. for loop over your threads
    allocate new thread_struct
    assign all variables 
    pthread_create(theads[0], NULL, function, thread_struct)
*/
}

void helper_v1(void* ts1_void){

    myThreadStruct_V1* ts1 = ts1_void;

    int begin = ts1->tid * ts1->chunkSize;
    int end = ts1->tid * ts1->chunkSize + ts1->chunkSize;

    for ( int i = begin; i < end; i++){
        ts1->y[ i ] = ts1->a * ts1->x[ i ] + ts1->y[ i ];     
    }    
}

/* Calculate SAXPY using pthreads, version 2. Place result in the Y vector */
void compute_using_pthreads_v2(float *x, float *y, float a, int num_elements, int num_threads)
{
    /* FIXME: Complete this function */

pthread_t* threadsv2 = (pthread_t *)malloc( num_threads * sizeof(pthread_t));
//pthread_t* threads[num_threads];
myThreadStruct_V2* ts2;

for(int i=0; i < num_threads; i++){
    ts2 = (myThreadStruct_V2*) malloc(sizeof(myThreadStruct_V2));
    ts2->tid = i;
    ts2->x = x;
    ts2->y = y;
    ts2->a = a;
    ts2->stride = num_threads;
    ts2->num_elements = num_elements;

    pthread_create(&threadsv2[i], NULL, (void*)helper_v2, ts2);


}

for(int i=0; i < num_threads; i++){
    pthread_join( threadsv2[i], NULL );
}
free(threadsv2);


    /*provided pseudo code*/

/*
    int stride = num_threads;
    while(tid < n) {
        y[tid] = a* x[tid] + y[tid];
        tid = tid + stride;
    }
*/
}

void helper_v2(void* ts2_void){

    myThreadStruct_V2* ts2 = ts2_void;

    while(ts2->tid < ts2-> num_elements){
        ts2->y[ts2->tid] = ts2->a * ts2->x[ts2->tid] + ts2->y[ts2->tid];
        ts2->tid = ts2->tid + ts2->stride;
    }

}

/* Perform element-by-element check of vector if relative error is within specified threshold */
int check_results(float *A, float *B, int num_elements, float threshold)
{
    int i;
    for (i = 0; i < num_elements; i++) {
        if (fabsf((A[i] - B[i])/A[i]) > threshold)
            return -1;
    }
    
    return 0;
}



