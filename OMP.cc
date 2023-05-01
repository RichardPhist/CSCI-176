#include<iostream>
#include<opencv2/highgui/highgui.hpp>
#include<ctime>
using namespace cv;//Declaring cv namespace
using namespace std;

void adding_Noise(Mat& image, int n){ //'adding_Noise' function//
    for (int x = 0; x < n; x++){ //initiating a for loop//
        int i = rand() % image.cols;//accessing random column//
        int j = rand() % image.rows;//accessing random rows//
        if (image.channels() == 1){ //apply noise to grayscale image//
            image.at<uchar>(j, i) = 0;//Changing the value of pixel//
        }
        if (image.channels() == 3){ //apply noise to RGB image//
            image.at<Vec3b>(j, i)[0] = ((image.at<Vec3b>(j,i)[0]+rand()%255) < 255) ? (image.at<Vec3b>(j,i)[0] + rand()%255)  : image.at<Vec3b>(j,i)[0] = 255;//Changing the value of first channel//

            image.at<Vec3b>(j, i)[1] = ((image.at<Vec3b>(j,i)[1]+rand()%255) < 255) ? (image.at<Vec3b>(j,i)[1] + rand()%255)  : image.at<Vec3b>(j,i)[1] = 255;//Changing the value of first channel//
            image.at<Vec3b>(j, i)[2] = ((image.at<Vec3b>(j,i)[2]+rand()%255) < 255) ? (image.at<Vec3b>(j,i)[2] + rand()%255)  : image.at<Vec3b>(j,i)[2] = 255;//C((image.at<Vec3b>(j,i)[0]+rand()%255) < 255) ? (image.at<Vec3b>(j,i)[0] + rand()%255)  : image.at<Vec3b>(j,i)[0] = 255hanging the value of first channel//
        }
    }
}

//

int main() {
    Mat image;//taking an image matrix//
    image = imread("sky.jpg");//loading an image//
    vector<int> compression_params;
    compression_params.push_back(IMWRITE_PNG_COMPRESSION);
    compression_params.push_back(9);
    int n = image.size().height * image.size().width;
    clock_t start_t, end_t;
    start_t = clock();
    adding_Noise(image, n/10);//calling the 'adding_Noise' function//
    end_t = clock();
    bool result = false;
    try{
        result = imwrite("output.jpg", image,compression_params);
    }
    catch(const cv::Exception& ex) {
        cout << "oops" << endl;
    }
    cout << end_t - start_t << endl;
    return (result) ? 0 : 1;
}