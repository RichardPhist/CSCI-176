//////////////////
//// Purpose: Estimate definite integral (or area under curve) using trapezoidal rule.
//// - this version uses double type thread function. 
////
//// Input:   a, b, n //assume n is evenly divisible by num_threads
//// Output:  estimate of integral from a to b of f(x) using n trapezoids
////
//// Compile/run: $> g++ -fopenmp homework3.cpp
////              $> xxx 4 //any number of threads
//////////////////

#include <cstdlib> //for atoi() and exit(0)
#include <iostream>
#include <omp.h>
using namespace std;

double f(double); //f(x)=x^2 
double Trap(double, double, int);

int main(int argc, char* argv[]) 
{
   double  global_result = 0.0;
   float startTime, endTime;
   double local_result;
   double  a, b; //left and right end points
   int     n;    //total number of trapezoids

   int thread_count = atoi(argv[1]); //command line arg
   cout<<"Enter a, b, and n\n";
   cin>>a>>b>>n;
   if(n % thread_count != 0) 
   { cerr<<"n should be evenly divisible by "<<thread_count<<endl;
     exit(0);
   }

  startTime = omp_get_wtime();

   #pragma omp parallel num_threads(thread_count)\
    reduction(+:global_result)
    local_result += Trap(a, b, n);
    #pragma omp barrier
    #pragma omp critical
      global_result += local_result;

   cout<<"With n="<<n<<" trapezoids, our estimate of integral from "
       <<a<<" to "<<b<<" is "<<global_result<<endl;
   cout << "Total time taken: " << (omp_get_wtime() - startTime) << endl;

  return 0;
}  /* main */

/////////////////// function f(x) = x * x ;
double f(double x) 
{
   return x*x;
}//f

////////////////// thread function for Trap
//// Purpose: Use trapezoidal rule to estimate definite integral
//// Input args:  
////    a, b : left, right endpoints
////    b: right endpoints
////    n: number of trapezoids
//// Output:
////    local_result: estimate of integral from a to b of f(x)
//////////////////
double Trap(double a, double b, int n) 
{
   double  h, x, my_result;
   double  local_a, local_b;
   int i, local_n;
   int my_rank = omp_get_thread_num();
   int thread_count = omp_get_num_threads(); //returns the count passed from pragma parallel

   h = (b-a)/n; 
   local_n = n/thread_count;  
   local_a = a + my_rank*local_n*h; 
   local_b = local_a + local_n*h; 
   my_result = (f(local_a) + f(local_b))/2.0; 
   for (i = 1; i <= local_n-1; i++) 
   { x = local_a + i*h;
     my_result += f(x);
   }

   my_result = my_result*h; 
   #pragma omp critical
    cout<<"Thread_"<<my_rank<<", local_result = "<<my_result<<endl;

  return my_result; 
}//
