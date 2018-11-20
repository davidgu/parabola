#include <iostream>
#include <time.h>
#include <thread>

#include "opencv2/opencv.hpp"
#include "opencv2/tracking.hpp"

#include "vector3.hpp"
#include "framebuffer.hpp"

#define ll long long

using namespace cv;

Mat dewarp(Mat &frame){
  Mat undistorted;
  Mat Distortion_Coefficients;
  Mat Camera_Matrix;

  FileStorage fs("../out_camera_data.xml",FileStorage::READ);
  fs["Distortion_Coefficients"] >> Distortion_Coefficients;
  fs["Camera_Matrix"] >> Camera_Matrix;
  fs.release();
  undistort(frame, undistorted, Camera_Matrix, Distortion_Coefficients);
  return undistorted;
}

int main(){

  VideoCapture cap;
  if(!cap.open(1))
    return 0;

  cap.set(CV_CAP_PROP_FRAME_WIDTH,640);
  cap.set(CV_CAP_PROP_FRAME_HEIGHT,480);
  cap.set(CV_CAP_PROP_AUTOFOCUS, 0);
  cap.set(CV_CAP_PROP_EXPOSURE, 0);

  Mat frame;
  Mat fixed;

  while(1){
    if( waitKey(10) == 27 ) break; // stop capturing by pressing ES 
    cap.read(frame);

    fixed = dewarp(frame);

    imshow("23",frame);
    imshow("2",fixed);
  }
}
