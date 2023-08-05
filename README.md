# Saxpy
1) The design of your multi-threaded implementations, using code or pseudo code to clarify the discussion

For compute_using_pthreads_v1() pseudocode:

1) To maintain track of threads, initialize an array of pthread_t objects.

2) For each thread, determine the chunk size.

3) Make a myThreadStruct_V1 object for each thread that has references to the X and Y vectors as well as the thread ID, the number of elements to be processed, the chunk size, the scalar value a, and the thread ID.

4) Each thread should be created by calling pthread_create() and providing the thread ID, a pointer to the myThreadStruct_V1 object, and a pointer to the helper function it will use.

5) Calculate the beginning and ending indices for the group of elements that the thread will process in the assistance function helper_v1().

6) Use the formula given to get SAXPY for the group of elements: Y[i] = a * X[i] + Y[i].

7) Call pthread_join() for each thread to wait for it to finish once all others have finished.

8) Any dynamically allocated memory must be freed.

For compute_using_pthreads_v2() pseudocode:

1) To maintain track of threads, initialize an array of pthread_t objects.

2) For each thread, determine the chunk size.

3) Make a myThreadStruct_V2 object for each thread that has references to the X and Y vectors as well as the thread ID, the number of elements to be processed, the chunk size, the scalar value a, and the stride.

4) Each thread should be created by calling pthread_create() and providing the thread ID, a pointer to the myThreadStruct_V2 object, and a pointer to the helper function it will use.

5) Determine the starting index for the group of elements that the thread will process using its thread ID and stride in the helper function helper_v2().

6) Use the following formula to get SAXPY for a group of elements: Y[i] = a * X[i] + Y[i].

7) Call pthread_join() for each thread to wait for it to finish once all others have finished.

8) Any dynamically allocated memory must be freed.

2) The performance achieved by 4, 8, and 16 threads, for 104 , 106 , and 108 elements, relative to the serial version.

For 10^4 elements:

o Serial version: 1 thread

o Parallel version with 4 threads: 1.271 / 0.379 = 3.35x faster

o Parallel version with 8 threads: 1.271 / 0.375 = 3.39x faster

o Parallel version with 16 threads: 1.271 / 0.375 = 3.39x faster
