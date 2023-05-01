#include <iostream>
#include <omp.h>

using namespace std;


void merge(int* global_list, int start, int mid, int end) {
    int* temp = (int*) malloc((end - start + 1) * sizeof(int));
    int i = start, j = mid + 1, k = 0;
    while (i <= mid && j <= end) {
        if (global_list[i] <= global_list[j]) {
            temp[k++] = global_list[i++];
        } 
        else {
            temp[k++] = global_list[j++];
        }
    }
    while (i <= mid) {
        temp[k++] = global_list[i++];
    }
    while (j <= end) {
        temp[k++] = global_list[j++];
    }
    for (i = start, k = 0; i <= end; i++, k++) {
        global_list[i] = temp[k];
    }
    free(temp);
}

void parallel_merge(int* global_list, int n, int thread_count) {
    int divisor = 2;
    int core_difference = 1;
    while (divisor <= thread_count) {
        #pragma omp barrier
        int sender_id = omp_get_thread_num() % divisor;
        int receiver_id = sender_id + core_difference;
        if (receiver_id < divisor) {
            int start = (n / thread_count) * receiver_id;
            int mid = (n / thread_count) * (receiver_id + 1) - 1;
            int end = mid + (n / thread_count);
            merge(global_list, start, mid, end);
        }
        divisor *= 2;
        core_difference *= 2;
    }
}
int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: ./program n p\n");
        return 1;
    }
    int n = atoi(argv[1]);
    int thread_count = atoi(argv[2]);
    int* list = (int*) malloc(n * sizeof(int));
    srand(time(NULL));
    for (int i = 0; i < n; i++) {
        list[i] = rand() % n;
    }
    int chunk_size = n/thread_count;
    #pragma omp parallel num_threads(thread_count)
    {
        int tid = omp_get_thread_num();
        int my_start = tid * chunk_size;
        int my_end = my_start + chunk_size - 1;
        int* local_list = (int*) malloc(chunk_size * sizeof(int));
        #pragma omp parallel for
        for (int i = my_start; i <= my_end; i++) {
            local_list[i - my_start] = list[i];
        }
        qsort(local_list, chunk_size, sizeof(int), [](const void* a, const void* b) 
                                                    {
                                                        int num_a = *(const int*)a;
                                                        int num_b = *(const int*)b;
                                                        if (num_a < num_b) {
                                                            return -1;
                                                        } else if (num_a > num_b) {
                                                            return 1;
                                                        } else {
                                                            return 0;
                                                        }
                                                    });
        #pragma omp barrier
        #pragma omp critical
        {
            for (int i = my_start; i <= my_end; i++) {
                list[i] = local_list[i - my_start];
            }
        }
        free(local_list);
    }
    parallel_merge(list,n,thread_count);

    printf("Sorted list:\n");
    for (int i = 0; i < n; i++) {
        printf("%d ", list[i]);
    }
    printf("\n");
    //free(list);
    return 0;
}