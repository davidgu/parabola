#include "opencv2/opencv.hpp"
#include "opencv2/tracking.hpp"
#include <iostream>
#include <time.h>
#include <thread>

using namespace cv;

const int CAMERA_1 = 0;
const int CAMERA_2 = 1;
const int CAMERA_3 = 2;
const int USE_THREADS = 0;

const std::string trackerList[8] = {"BOOSTING", "MIL", "KCF", "TLD","MEDIANFLOW", "GOTURN", "MOSSE", "CSRT"};
const int trackerSelection = 4;

void get_fps(VideoCapture cap){
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
  std::cout<< "FPS: " << num_frames / seconds << std::endl;
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

void process_frame(VideoCapture cap, Tracker* tracker){
  Mat frame;
  cap>>frame;
  Rect2d bbox(287, 23, 86, 320);
  // If tracking is successful, draw the bounding box
  bool ok = tracker->update(frame, bbox);
  if(ok){
    rectangle(frame, bbox, Scalar( 255, 0, 0 ), 2, 1 );
  }
  else{
    std::cout << "Tracking failure!" << std::endl;
  }

  imshow("Tracking", frame);
}

int main(int argc, char** argv){
  VideoCapture cap;
  if(!cap.open(CAMERA_1))
    return 0;

  cap.set(CV_CAP_PROP_FRAME_WIDTH,640);
  cap.set(CV_CAP_PROP_FRAME_HEIGHT,480);

  std::thread t1 (get_fps,cap);

  Ptr<Tracker> tracker = get_tracker(trackerList[trackerSelection]);

  // Initialize tracking and display bounding box
  Mat frame;
  cap >> frame;
  Rect2d bbox(287, 23, 86, 320); 
  rectangle(frame, bbox, Scalar( 255, 0, 0 ), 2, 1 ); 
  tracker->init(frame, bbox);
  imshow("Tracking", frame);

  while(true){
    if( waitKey(10) == 27 ) break; // stop capturing by pressing ESC
    if(USE_THREADS){
      std::thread t2(process_frame,cap,tracker);
      t2.detach();
    }else{
      cap >> frame;

      // If tracking is successful, draw the bounding box
      bool ok = tracker->update(frame, bbox);
      if(ok){
        rectangle(frame, bbox, Scalar( 255, 0, 0 ), 2, 1 );
      }
      else{
        std::cout << "Tracking failure!" << std::endl;
      }

      imshow("Tracking", frame);
    }
  }
  // the camera will be closed automatically upon exit
  // cap.close();
  t1.join();
  return 0;
}
