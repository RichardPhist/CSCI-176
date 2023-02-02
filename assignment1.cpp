/////// Fibonacci sequence recursive vs. iterative
#include <unistd.h> //for fork, pipe, wait
#include <sys/wait.h>
#include <cstdlib> //for exit(0)
#include <cstring>
#include <iostream>
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
        for (int i = 3; i < N; i++)
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
    int pid, status, i;
    int p1[2], p2[2]; // pipe descriptors
    int response, response2;
    char s[MAX];
    pipe(p1);
    pipe(p2);
    for (i = 1; i < 3; i++)
    {
        pid = fork();
        if (pid == 0 && i == 1) // child process1
        {                       // close(p1[0]);
            // close(p2[1]);
            cout << " Menu: \n";
            cout << " ------------- \n";
            cout << " 1. display Hello " << endl;
            cout << " 2. display Bye " << endl;
            cout << " ------------- \n";
            cout << " choice ( 1-2 )?\n";
            cin >> response;
            write(p1[1], &response, sizeof(int));
            read(p2[0], s, MAX);
            cout << s << endl;
            // close(p1[1]);
            // close(p2[0]);
            exit(0);
        }                            // if
        else if (pid == 0 && i == 2) // child process2
        {                            // close(p1[1]);
            // close(p2[0]);
            read(p1[0], &response2, sizeof(int));
            switch (response2)
            {
            case 1:
                strcpy(s, "Hello\n");
                break;
            case 2:
                strcpy(s, "Bye\n");
                break;
            default:
                strcpy(s, "Try option 1 or 2.\n");
                break;
            }
            write(p2[1], s, MAX);
            // close(p1[0]);
            // close(p2[1]);
            exit(0);
        } // elseif
    }     // for_i
    // Now wait for the child processes to finish
    for (i = 1; i <= 2; i++)
    {
        pid = wait(&status);
        cout << "Child (pid=" << pid << ") exited, status=" << status << endl;
    }
    return 0;
} // main