/*
Optimal Parallel Prefix Computation Model
Carlo Quitorio CSCI 176 3/12/23

Demonstrate and practice use of pThreads and semaphores C++ version.
Specifically: semaphore solution to producer-consumer synchronization.

Each thread sends a message to the next thread, then displays
the messsage reveived from the previous thread.
Uses named semaphores

TO USE:
g++ -lpthread program3.cpp
./a.exe 4   or any
*/

#include <pthread.h>
#include <semaphore.h>
#include <cstdlib>
#include <string>
#include <sstream>
#include <iomanip>
#include <time.h>
#include <iostream>
#include <sys/time.h>  //for macro GET_TIME(double)

////time checking macro --needs <sys/time.h>
#define GET_TIME(now)\
{ struct timeval t; gettimeofday(&t, NULL);\
  now = t.tv_sec + t.tv_usec/1000000.0; }

using namespace std;

const int ARR_SIZE = 500000017; //500000017
double *GLOBAL_ARRAY = new double[ARR_SIZE]; //dynamically allocate the array

int thread_count; //test with 1, 2, 4, and 8

sem_t** semArray; //array of semaphore pointers

void* prefixSum(void* args);

int main(int argc, char* argv[]){

    //separate list that holds the thread ranks
    int* listThread = new int[thread_count];

    //Initialize data for global array
    for(int i = 0; i < ARR_SIZE; i++){
        GLOBAL_ARRAY[i] = i+1;
    }
    //cout << "Done initializing global array." << endl;

    long thread_id;
    thread_count = atoi(argv[1]);

    pthread_t myThreads[thread_count];
    semArray = new sem_t*[thread_count];

    //initialize semaphores
    for(thread_id = 0; thread_id < thread_count; thread_id++){
        semArray[thread_id] = new sem_t;
        sem_init(semArray[thread_id], 0, 0);
    }

    //start time before call of prefixSum function
    //clock_t start_t = clock();
    double start_time, end_time; //for time checking
    GET_TIME(start_time); //time checking starts

    //loop that creates the threads and passes the thread rank to the prefixSum
    for(thread_id = 0; thread_id < thread_count; thread_id++){
        cout << "Thread_" << thread_id << " is created." << endl;
        listThread[thread_id] = thread_id;
        pthread_create(&myThreads[thread_id], NULL, prefixSum, (void*) &listThread[thread_id]);
    }

    //threads rejoin after they're done
    for(int i = 0; i < thread_count; i++){ 
        pthread_join(myThreads[i], NULL);
    }

    //close semaphores after they're done
    for(thread_id = 0; thread_id < thread_count; thread_id++){
        sem_close(semArray[thread_id]);
    }

    //clock_t end_t = clock(); //end time after threads join
    GET_TIME(end_time); //time checking ends

    printf ("prefix_sum[0]            = %.8f\n", GLOBAL_ARRAY[0]);
    printf ("prefix_sum[1,000,000]    = %.8e\n", GLOBAL_ARRAY[1000000]);
    printf ("prefix_sum[10,000,000]   = %.8e\n", GLOBAL_ARRAY[10000000]);
    printf ("prefix_sum[100,000,000]  = %.8e\n", GLOBAL_ARRAY[100000000]);
    printf ("prefix_sum[200,000,000]  = %.8e\n", GLOBAL_ARRAY[200000000]);
    printf ("prefix_sum[300,000,000]  = %.8e\n", GLOBAL_ARRAY[300000000]);
    printf ("prefix_sum[400,000,000]  = %.8e\n", GLOBAL_ARRAY[400000000]);
    printf ("prefix_sum[500,000,000]  = %.8e\n", GLOBAL_ARRAY[500000000]);
    cout<<"++++ time taken (sec) = "<<end_time - start_time<<endl;
    //cout << "Execution Time: " << (double)(end_t - start_t)/CLOCKS_PER_SEC << "s" << endl;

    //delete all arrays to avoid memory leaks
    delete semArray; 
    delete GLOBAL_ARRAY;
    delete listThread;

    return 0;
}

void* prefixSum(void* args){ //adds the current value at index with the value at index before it
    //cout << "INSIDE PREFIX" << endl;
    int problemSize = ARR_SIZE / thread_count;
    int remainder = ARR_SIZE % thread_count;

    int rank = *(int*) args;
    int dest = (rank + 1) % thread_count;
    double sum = 0.0;

    //find start and end indexes
    int start = rank * problemSize;
    if(rank > 0 && rank < remainder){
        start += rank;
    }
    else if(rank > 0 && rank >= remainder){
        start += remainder;
    }
    int end = start + problemSize - 1;
    if(rank < remainder){
        end++;
    }

    //First stage: does prefix sum
    for(int i = start; i <= end; i++){
        sum += GLOBAL_ARRAY[i];
        GLOBAL_ARRAY[i] = sum;
    }
    //cout << "DATA RANK: " << rank << endl;

    //Second Stage: Synchronization
    if(rank == 0){ //notify to First Semaphore
        //cout << "IF STATEMENT RANK_" << rank << " posting to: " << dest << endl;
        cout << "Thread: " << rank << " start_ind=" << start  << ", " << "end_ind=" << end << endl;
        sem_post(semArray[dest]); //DONE
    }

    else{ //all other semaphores

        sem_wait(semArray[rank]);
        cout << "Thread: " << rank << " start_ind=" << start  << ", " << "end_ind=" << end << endl;

        //Third Stage: adding thread i - 1's highest index to array
        sum = GLOBAL_ARRAY[start - 1];
        for(int i = start; i <= end; i++){
            GLOBAL_ARRAY[i] += sum;
        }
        //cout << "ELSE STATEMENT RANK_" << rank << " posting to: " << dest <<endl;
        sem_post(semArray[dest]);
    }

    sem_close(semArray[rank]); //close semaphore after posting
    pthread_exit(NULL); //thread exits after it's done
}
