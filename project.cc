/////////////////////////////////////////////
////
//// 
////
//// g++ project.cc `pkg-config --cflags --libs opencv` -fopenmp
//// ./a.exe 4 
//// 4 is thread count
/////////////////////////////////////////////


#include<iostream>
#include<opencv2/highgui/highgui.hpp>
#include <omp.h>

using namespace cv;//Declaring cv namespace
using namespace std;
#define inputred image.at<Vec3b>(j,i)[2]
#define inputgreen image.at<Vec3b>(j,i)[1]
#define inputblue image.at<Vec3b>(j,i)[0]

void adding_Noise(Mat& image, int start, int end){ //'adding_Noise' function//
    for (int i = start; i < end+1 ; i++){ //initiating a for loop//
        for(int j = 0; j <  image.size().height -1; j++){
            int redval = (inputred * 0.393) + (inputgreen * 0.769) + (inputblue * 0.189);
            int greenval = (inputred * 0.349) + (inputgreen * 0.686) + (inputblue *0.168);
            int blueval = (inputred * 0.272) + (inputgreen * 0.534) + (inputblue *0.131);

            inputred = (redval> 255) ? 255 : redval;
            inputgreen = (greenval > 255)? 255 : greenval;
            inputblue =  (blueval > 255)? 255 : blueval;
        }
    }
}

//

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: ./program n p\n");
        return 1;
    }
    string name = argv[2];
    int tc = atoi(argv[1]); // thread count
    Mat image;//taking an image matrix//
    image = imread(name);//loading an image//
    int n = image.size().height * image.size().width;
    int cs = image.size().width/tc; // chunk size

    bool result = false;
    float startTime = omp_get_wtime();

    #pragma omp parallel num_threads(tc)
    {
        int tid = omp_get_thread_num();
        int start = (image.size().width / tc) * tid;
        int end = start + cs -1;
        adding_Noise(image, start, end);//calling the 'adding_Noise' function//

        try{
            result = imwrite("output.jpg", image);
        }
        catch(const cv::Exception& ex) {
            cout << "oops" << endl;
        }
    }

    float endTime = omp_get_wtime();

    cout << "Time taken: " << (endTime - startTime) << endl;
    return (result) ? 0 : 1;
}