//////////////////////////////////////////////////////////////
//// Carlo Quitorio
//// CSCI 176 Program 4
//// Merge Sort with threading using openMP
////
//// OpenMP allows us to use threading more easily, i.e. more high level
//// Need to use barriers and critical to synchronize threads.
//// Included extra functions to help inbuilt quicksort.
//// 
//// $> g++ -fopenmp program4.cpp
//// $>./a.exe 4 10000000
//// 
//// 4 is thread count, can be any
//// 10000000 is size of array, can be any
//////////////////////////////////////////////////////////////

#include <cstdlib> //for aoti()
#include <iostream>
#include <omp.h>
using namespace std;

int thread_count;
int n;

//merges each local list into the global list
void merge(int* global_list, int start, int mid, int end){
    int* temp = new int[end - start + 1];
    int i = start;
    int j = mid + 1;
    int k = 0;
    while(i <= mid && j <= end){
        if(global_list[i] <= global_list[j]){
            temp[k++] = global_list[i++];
        }
        else{
            temp[k++] = global_list[j++];
        }
    }
    while(i <= mid){
        temp[k++] = global_list[i++];
    }
    while(j <= end){
        temp[k++] = global_list[j++];
    }
    for(i = start, k = 0; i <= end; i++, k++)
    {
        global_list[i] = temp[k];
    }
    free (temp);
}

int Compare(const void* a_p, const void* b_p) //needed for inbuilt quicksort
{ 
    int a = *((int*)a_p);
    int b = *((int*)b_p);

    if (a < b) return -1;
    else if (a == b) return 0;
    else return 1; //(a > b)
}//Compare

int check_sorted (int list1[], int n) //checks if global array is sorted
{ 
    for (int i=0; i<n-1; i++){
        if (list1[i] > list1[i+1]){
            return 0; //not sorted
        }
    }
    return 1; //sorted
}

int main(int argc, char* argv[]) 
{
    thread_count = atoi(argv[1]); //command line arg1 for P
    n = atoi(argv[2]); //command line arg2 for list size
    int problemSize = n/thread_count;
    int* main_list = new int[n]; //global dynamic array

    //initializes main_list
    for (int i = 0; i < n; i++){
        main_list[i] = rand()%n+1;
    }

    float startTime = omp_get_wtime();

    #pragma omp parallel num_threads(thread_count)
    {
        int rank = omp_get_thread_num();
        int start = rank * problemSize;
        int end = start + problemSize - 1;
        int* local_list = new int[problemSize]; //temporary dynamic array

        #pragma omp parallel for
            for(int i = start; i <= end; i++){
                local_list[i-start] = main_list[i];
            }

        //needed to print threads in order
        #pragma omp barrier
        #pragma omp critical
        {
            if(problemSize < 101){
                cout << "\nunsorted local_list_" << rank << ": ";
                for(int i = 0; i < problemSize; i++){
                    cout << local_list[i] << " ";
                }
            }
        }

        //sorts each local thread
        qsort(local_list, problemSize, sizeof(int), Compare); //GNU library quick sort, needs entire array

        //needed to print local threads in order
        #pragma omp barrier
        #pragma omp critical
        {
            if(problemSize < 101){
                cout << "\nsorted local_list_" << rank << ": ";
                for(int i = 0; i < problemSize; i++){
                    cout << local_list[i] << " ";
                }
            }
        }

        for(int i = start; i <= end; i++){
            main_list[i] = local_list[i-start];
        }
        free(local_list); //free up memory

        //start merging the local lists
        int divisor = 2;
        int core_diff = 1;
        while(divisor <= thread_count){
            #pragma omp barrier
            int thread_num = omp_get_thread_num();
            if(thread_num % divisor == 0){
                int sender_id = thread_num;
                int receiver_id = thread_num + core_diff;
                int start = (n/thread_count) * thread_num;
                int end = ((n/thread_count) * (receiver_id + 1)) - 1;
                int mid = (start + end) / 2;
                //cout << start << " " << mid << " " << end << " " <<  endl;
                merge(main_list, start, mid, end);
            }
        divisor *= 2;
        core_diff  = divisor - 1;
        }
    }
    
    float endTime = omp_get_wtime();

    //display the final sorted main_list
    if(n < 101){
        cout<<"\nsorted main_list: "<<endl;
        for (int i = 0; i<n; i++){
        cout << main_list[i]<<" ";
        }
    }

    cout << endl;

    if (check_sorted(main_list, n) == 1){
        cout << "**verified that main_list is sorted." << endl;
    }

    else{
        cout << "**list1 is not completely sorted - check program!" << endl;
    }

    cout << "Time taken: " << (endTime - startTime) << endl;

    free(main_list); //free memory

    return 0;
  
}//main
