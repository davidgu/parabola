#include "opencv2/opencv.hpp"
#include <iostream>
#include <time.h>

using namespace cv;

const int CAMERA_1 = 0;
const int CAMERA_2 = 1;
const int CAMERA_3 = 2;

double get_fps(VideoCapture cap){
    int num_frames = 120;
    time_t start, end;
    Mat frame;

    // Time capturing specified number of frames
    time(&start);
    for(int i = 0; i < num_frames; i++)
    {
        cap >> frame;
    }
    time(&end);
     
    // Calculate FPS
    double seconds = difftime (end, start);
    return num_frames / seconds;
}

int main(int argc, char** argv){
  VideoCapture cap;
  if(!cap.open(CAMERA_1))
    return 0;

  cap.set(CV_CAP_PROP_FRAME_WIDTH,640);
  cap.set(CV_CAP_PROP_FRAME_HEIGHT,480);

  std::cout << "FPS: " << get_fps(cap) << std::endl;

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