#include <iostream>
#include <time.h>
#include "opencv2/opencv.hpp"
#define ll long long
// note: I am loading it in with open cv because to maintain consistant image quality
using namespace cv; 
long timediff(clock_t t1, clock_t t2) {
  long elapsed;
  elapsed = ((double)t2 - t1) / CLOCKS_PER_SEC * 1000;
  return elapsed;
}
int main(int argc, char** argv){ 
  ll cap_duration = 30000;
  const int cur_camera = 1;
  
  VideoCapture cap;

  cap.set(CV_CAP_PROP_FRAME_WIDTH,1920);
  cap.set(CV_CAP_PROP_FRAME_HEIGHT,1080);


  if(!cap.open(cur_camera)) return 0;
  if(argc == 1) std::cout<<"no time specified defaulting to 1 second"<<std::endl;
  else if(argc > 2) std::cout<<"too many times specified defaulting to 1 second"<<std::endl;
  else{
    cap_duration = ((ll)argv[1])*1000;
  }

  clock_t start_time, cur_time;
  start_time = clock();
  cur_time = clock();
  Mat frames[10000];
  int frame_count = 0;
  std::cout<<"capturing frames"<<std::endl;
  while(timediff(start_time, cur_time) <= cap_duration && frame_count < 1000){
    //if( waitKey(10) == 27 ) break; // stop capturing by pressing ESC 
    waitKey(10);
    cap.read(frames[frame_count]);
    frame_count++;
    cur_time = clock();
  }
  std::cout<<"ssaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framessaving framesaving frames"<<std::endl;
  for(int i = 0; i < frame_count;i++){
    imwrite("photos/" + std::to_string(clock()) +".jpg", frames[i]);
  }

  return 0; 
} 
