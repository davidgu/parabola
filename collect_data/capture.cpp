#include <iostream>
#include <time.h>
#include "opencv2/opencv.hpp"
 // note: I am loading it in with open cv because to maintain consistant image quality
using namespace cv; 

int main(int argc, char** argv){ 
  double time = 1000; // ik I need the time class for this
  if(argc == 1) std::cout<<"no time specified defaulting to 1 second"<<std::endl;
  else if(argc > 2) std::cout<<"too many times specified defaulting to 1 second"<<std::endl;
  else{
    std::cout << argv[i] << "\n";
  }

  return 0; 
} 
