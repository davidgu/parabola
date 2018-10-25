#include <iostream>
#include <time.h>
#include <thread>

#include "opencv2/opencv.hpp"
#include "opencv2/tracking.hpp"

#include "vector3.h"
#include "framebuffer.h"

#define ll long long

using namespace cv;

const int CAMERA_1 = 0; // cam 0 is the webcam
const int CAMERA_2 = 1;
const int CAMERA_3 = 2;
const int MULTITHREAD = 0;

const std::string trackerList[8] = {"BOOSTING", "MIL", "KCF", "TLD","MEDIANFLOW", "GOTURN", "MOSSE", "CSRT"};
const int trackerSelection = 4;

FrameBuffer frameBuffer;

double get_fps(VideoCapture cap){
  int num_frames = 120;
  time_t start, end;
  Mat frame;

  // Time capturing specified number of frames
  time(&start);
  for(int i = 0; i < num_frames; i++){
    cap.read(frame);
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

// Comparasion of speed between >> and .read
// .read is faster

long timediff(clock_t t1, clock_t t2) {
  long elapsed;
  elapsed = ((double)t2 - t1) / CLOCKS_PER_SEC * 1000;
  return elapsed;
}

void test_write_speed(VideoCapture cap){
  Mat stream_test;
  Mat read_test;

  clock_t start1, end1, start2, end2;
  start1 = clock();
  cap>>stream_test;
  end1 = clock();
  start2 = clock();
  cap.read(read_test);
  end2 = clock();
  std::printf("%ld\n",timediff(start1, end1));
  std::printf("%ld\n",timediff(start2, end2));
}

bool custom_process_frame(VideoCapture cap){
  if( waitKey(10) == 27 ) return 1; // stop capturing by pressing ESC
  Mat frame;
  cap.read(frame);

  Mat lower_red_hue_range;
  Mat upper_red_hue_range;
  //inRange(frame, Scalar(0, 100, 100), Scalar(10, 255, 255), lower_red_hue_range);
  inRange(frame, Scalar(10, 20, 20), Scalar(180, 40, 40), upper_red_hue_range);

  Mat red_hue_image = upper_red_hue_range;
  //addWeighted(lower_red_hue_range, 1.0, upper_red_hue_range, 1.0, 0.0, red_hue_image);
  //GaussianBlur(red_hue_image, red_hue_image, Size(9, 9), 2, 2);

  imshow("Tracking", red_hue_image);
  /* std::vector<Vec3f> circles;
     HoughCircles(red_hue_image, circles, CV_HOUGH_GRADIENT, 1, red_hue_image.rows/8, 100, 20, 0, 0);*/
  return 0;
}

void process_frame(VideoCapture cap, Ptr<Tracker> tracker, Rect2d bbox){
  Mat frame;
  cap.read(frame);

  // If tracking is successful, draw the bounding box
  bool ok = tracker->update(frame, bbox);
  if(ok){
    rectangle(frame, bbox, Scalar( 255, 0, 0 ), 2, 1 );
    printf("x: %f y: %f",bbox.x, bbox.y);
    // TODO: Determine which dimensions each camera is responsible for watching
    // TODO: Determine how to resolve conflicts between cameras
    // TODO: Write data to shared framebuffer
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

  Ptr<Tracker> tracker = get_tracker(trackerList[trackerSelection]);
  Rect2d bbox(287, 23, 86, 320); 

  // If multithread is enabled, each camera view will be read by an independent
  // thread, and processed data will be written to shared memory.
  if(MULTITHREAD){
    std::thread c1(process_frame,cap,tracker,bbox);
    std::thread c2(process_frame,cap,tracker,bbox);
    std::thread c3(process_frame,cap,tracker,bbox);

    // Read position data from shared FrameBuffer and prepare data to be sent
    // to AR client
    while(true){

    }
  }
  else{
    Mat frame;
    cap.read(frame);
    rectangle(frame, bbox, Scalar( 255, 0, 0 ), 2, 1 ); 
    tracker->init(frame, bbox);
    imshow("Tracking", frame);
    while(true){
      /*if(custom_process_frame(cap)){
        break;
        }*/

      if( waitKey(10) == 27 ) break; // stop capturing by pressing ESC
      cap.read(frame);

      // If tracking is successful, draw the bounding box
      bool ok = tracker->update(frame, bbox);
      if(ok){
        rectangle(frame, bbox, Scalar( 255, 0, 0 ), 2, 1 );
        printf("x: %f y: %f\n",bbox.x, bbox.y);
      }
      else{
        std::cout << "Tracking failure!" << std::endl;
      }
      imshow("Tracking", frame);
    }
  }
  return 0;
}
