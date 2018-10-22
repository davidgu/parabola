#include "opencv2/opencv.hpp"
#include <iostream>
using namespace cv;
int main(int argc, char** argv){
  VideoCapture cap;
  if(!cap.open(1))
    return 0;

  cap.set(CV_CAP_PROP_FRAME_WIDTH,640);
  cap.set(CV_CAP_PROP_FRAME_HEIGHT,480);

  while(true){
    Mat frame;
    cap >> frame;
    if( frame.empty() ) break; // end of video stream
    imshow("this is you, smile! :)", frame);
    if( waitKey(10) == 27 ) break; // stop capturing by pressing ESC
  }
  // the camera will be closed automatically upon exit
  // cap.close();
  return 0;
}
