/////// Fibonacci sequence recursive vs. iterative
#include <unistd.h> //for fork, pipe, wait
#include <sys/wait.h>
#include <cstdlib> //for exit(0)
#include <cstring>
#include <iostream>
#include <time.h>
using namespace std;
#define MAX 100

int fibo_iter(int N) //iterative fib
{
    int f1 = 1;
    int f2 = 1;
    int temp;
    if (N == 1 || N == 2)
        return 1;
    else
    {
        for (int i = 3; i <= N; i++)
        {
            temp = f1 + f2;
            f1 = f2;
            f2 = temp;
        }
        return temp;
    }
}

int fibo_rec(int N) //recursive fib
{
    if (N == 1 || N == 2)
        return 1;
    else
        return (fibo_rec(N - 1) + fibo_rec(N - 2));
}

int main()
{
    clock_t start_t, end_t;
    int pid, status, i, user;
    int p1[2], p2[2], p3[2], p4[2]; // pipe descriptors
    char s[MAX];
    pipe(p1);
    pipe(p2);
    pipe(p3);
    pipe(p4);

    cout << "Please input a number: " << endl;
    cin >> user;
    for (i = 1; i < 4; i++) //loops 3 times
    {
        pid = fork();
        if(pid == 0)
        {
            if(i == 1) //if 1st child
            {
                double t2, t3;
                close(p1[0]);
                close(p2[0]);

                write(p1[1], &user, sizeof(user)); //send N to 2nd child
                write(p2[1], &user, sizeof(user)); //send N to 3rd child

                read(p3[0], &t2, sizeof(t2)); //get run time from 2nd child
                read(p4[0], &t3, sizeof(t3)); //get run time from 3rd child
                cout << "Run time of RECC Fib is: " << t2 << " seconds." << endl;
                cout << "Run time of ITT Fib is:  " << t3 << " seconds." << endl;
                break;
            }

            if(i == 2) //if 2nd child
            {
                int recursive;
                double total_time;
                close(p1[1]);
                close(p3[0]);

                read(p1[0], &recursive, sizeof(recursive)); //get number from 1st child

                start_t = clock();
                recursive = fibo_rec(recursive); //calculate recursive fib
                end_t = clock();
                total_time = (double)(end_t - start_t) / CLOCKS_PER_SEC;

                cout << user << " RECC Fibonocci number is: " << recursive << " inside 2nd child" << endl;
                write(p3[1], &total_time, sizeof(total_time)); //send time to 1st child
                break;
            }

            if(i == 3) //if 3rd child
            {
                int iterative;
                double total_time;
                close(p2[1]);
                close(p4[0]);

                read(p2[0], &iterative, sizeof(iterative)); //get number from 1st child

                start_t = clock();
                iterative = fibo_iter(iterative);
                end_t = clock();
                total_time = (double)(end_t - start_t) / CLOCKS_PER_SEC;

                cout << user << " ITT Fibonocci number is " << iterative << " inside 3rd child" << endl;
                write(p4[1], &total_time, sizeof(total_time)); //send time to 1st child
                break;
            }
        }
    }     // for_i

    for(int i = 1; i < 4; i++)
    {
        wait(NULL);
    }
    return 0;
} // main