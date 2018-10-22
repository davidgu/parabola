#include "opencv2/opencv.hpp"
#include "opencv2/tracking.hpp"
#include <iostream>
#include <time.h>

using namespace cv;

const int CAMERA_1 = 0;
const int CAMERA_2 = 1;
const int CAMERA_3 = 2;

const std::string trackerList[8] = {"BOOSTING", "MIL", "KCF", "TLD","MEDIANFLOW", "GOTURN", "MOSSE", "CSRT"};
const int trackerSelection = 2;

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

Ptr<Tracker> get_tracker(std::string trackerType){
  // Smart pointer to Tracker object
  Ptr<Tracker> tracker;
  if (trackerType == "BOOSTING")
      tracker = TrackerBoosting::create();
  if (trackerType == "MIL")
      tracker = TrackerMIL::create();
  if (trackerType == "KCF")
      tracker = TrackerKCF::create();
  if (trackerType == "TLD")
      tracker = TrackerTLD::create();
  if (trackerType == "MEDIANFLOW")
      tracker = TrackerMedianFlow::create();
  if (trackerType == "GOTURN")
      tracker = TrackerGOTURN::create();
  if (trackerType == "MOSSE")
      tracker = TrackerMOSSE::create();
  if (trackerType == "CSRT")
      tracker = TrackerCSRT::create();
  return tracker;
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