//////////////////
//// Purpose: Estimate definite integral (or area under curve) using trapezoidal rule.
//// - this version uses void type thread function, which uses global_result as ref para. 
////
//// Input:   a, b, n //assume n is evenly divisible by num_threads
//// Output:  estimate of integral from a to b of f(x) using n trapezoids
////
//// Compile/run: $> g++ -fopenmp -o xxx omp-trap1.cpp
////              $> xxx 4 //any number of threads
//////////////////

#include <cstdlib> //for atoi() and exit(0)
#include <iostream>
#include <omp.h>
using namespace std;

double f(double); //f(x)=x^2 
void Trap(double, double, int, double&);

int main(int argc, char* argv[]) 
{
   double  global_result = 0.0;
   float startTime, endTime;
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

   #pragma omp parallel num_threads(thread_count) 
    Trap(a, b, n, global_result); //global_result is ref para

   endTime = omp_get_wtime();

   cout<<"With n="<<n<<" trapezoids, our estimate of integral from "
       <<a<<" to "<<b<<" is "<<global_result<<endl;

   //cout << endTime << " " << startTime << endl;

   cout << "Total time taken: " << (endTime - startTime) << endl;

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
//// Output arg (ref para):
////    globla_result: estimate of integral from a to b of f(x)
//////////////////
void Trap(double a, double b, int n, double& global_result) 
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

   #pragma omp critical 
    global_result += my_result; 
}//
