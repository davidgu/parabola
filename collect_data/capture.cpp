#include <iostream>
#include <sys/time.h>
#include "opencv2/opencv.hpp"
#define ll long long
 // note: I am loading it in with open cv because to maintain consistant image quality
using namespace cv; 

int main(int argc, char** argv){ 
  ll cap_duration = 1000000000;
  if(argc == 1) std::cout<<"no time specified defaulting to 1 second"<<std::endl;
  else if(argc > 2) std::cout<<"too many times specified defaulting to 1 second"<<std::endl;
  else{
    cap_duration = ((ll)argv[1])*1000000000;
  }

  struct timespec end_time, cur_time;
  clock_gettime(CLOCK_MONOTONIC,&end_time);
  end_time.tv_nsec += cap_duration;
  while(clock_gettime(CLOCK_MONOTONIC,&cur_time).tv_nsec <end_time.tv_nsec){
    
  }

  return 0; 
} 
