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
  ll cap_duration = 1000;
  const int cur_camera = 0;


  VideoCapture cap;
  if(!cap.open(cur_camera)) return 0;
  if(argc == 1) std::cout<<"no time specified defaulting to 1 second"<<std::endl;
  else if(argc > 2) std::cout<<"too many times specified defaulting to 1 second"<<std::endl;
  else{
    cap_duration = ((ll)argv[1])*1000;
  }

  clock_t start_time, cur_time;
  start_time = clock();
  cur_time = clock();
  Mat frame;
  while(timediff(start_time, cur_time) <= cap_duration){
    //if( waitKey(10) == 27 ) break; // stop capturing by pressing ESC 
    std::cout<<cur_time<<std::endl;
    waitKey(10);
    cap.read(frame);
    imshow("capturing",frame);
    cur_time = clock();
  }

  return 0; 
} 
