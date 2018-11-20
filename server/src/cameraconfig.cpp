#include <string>
#include "cameraconfig.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/tracking.hpp"
#include "vector3.hpp"

CameraConfig::CameraConfig(int i, Vector3 p){
  index = i;
  position = p;
}
CameraConfig::CameraConfig(){
  index = -1;
  position = Vector3();
}

int CameraConfig::get_index(){
  return index;
}

Vector3 CameraConfig::get_position(){
  return position;
}

void CameraConfig::load_from_file(std::string path){
  // Unimplemented
}

int CameraConfig::countCameras() {
  cv::VideoCapture temp_camera;
  int maxTested = 4;
  for (int i = 0; i < maxTested; i++){
    cv::VideoCapture temp_camera(i);
    bool res = (!temp_camera.isOpened());
    temp_camera.release();
    if (res) {
      return i;
    }
  }
  return maxTested;
}

int CameraConfig::build_camera_config(){
  // Cycles through the cameras to find which one it is
  // Also states how far the cameras are from the middle
  double sideCamHeights = 1.4;
  /*int numCams = countCameras();
    if(numCams != 4){
    std::cout<<"Four cameras need to be connected. The number of cameras are: "<<numCams<<std::endl;
    return;
    }*/
  std::cout<<"4 Cameras connected. Camera Location Detection Running."<<std::endl;

  // idx 0 = top, idx 1 = up, idx 2 = right
  int calibratedCamIdx[4]; // Each index describes the camera number


  // I can put this in an array

  // cv::VideoCapture capArr[4];
 
  cv::VideoCapture cap0;
  cv::VideoCapture cap1;
  cv::VideoCapture cap2;
  cv::VideoCapture cap3;
  cap0.open(0);
  cap1.open(1);
  cap2.open(2);
  cap3.open(3);

  cap0.set(CV_CAP_PROP_FRAME_WIDTH,640);
  cap0.set(CV_CAP_PROP_FRAME_HEIGHT,480);
  cap0.set(CV_CAP_PROP_AUTOFOCUS, 0);
  cap0.set(CV_CAP_PROP_EXPOSURE, 0);

  cap1.set(CV_CAP_PROP_FRAME_WIDTH,640);
  cap1.set(CV_CAP_PROP_FRAME_HEIGHT,480);
  cap1.set(CV_CAP_PROP_AUTOFOCUS, 0);
  cap1.set(CV_CAP_PROP_EXPOSURE, 0);

  cap2.set(CV_CAP_PROP_FRAME_WIDTH,640);
  cap2.set(CV_CAP_PROP_FRAME_HEIGHT,480);
  cap2.set(CV_CAP_PROP_AUTOFOCUS, 0);
  cap2.set(CV_CAP_PROP_EXPOSURE, 0);

  cap3.set(CV_CAP_PROP_FRAME_WIDTH,640);
  cap3.set(CV_CAP_PROP_FRAME_HEIGHT,480);
  cap3.set(CV_CAP_PROP_AUTOFOCUS, 0);
  cap3.set(CV_CAP_PROP_EXPOSURE, 0);





  int curCam = 0;
  int camNumToIdx = -1;
  cv::Mat frames[4];
  int curCamShowing = 1;
  while(curCam < 3){
    if(curCamShowing == 0){
      cap0.read(frames[0]);
    }else if(curCamShowing == 1){
      cap1.read(frames[1]);
    }else if(curCamShowing == 2){
      cap2.read(frames[2]);
    }else if(curCamShowing == 3){
      cap3.read(frames[3]);
    }else{
      std::cout<<"Error: You are selecting a camera outside of the limits"<<std::endl;
    }
    int key = cv::waitKey(10);
    if(key == 27) return 1; // stop capturing by pressing ES
    else if(key == 13){ // ENTER pressed, selecting camera for curCam

    } else if(key == 32){ // SPACE pressed, cycle through the cameras
      curCamShowing++;
      if(curCamShowing == 4) curCamShowing = 0;
    }
    else if(key != -1)std::cout<<key<<std::endl;


    // Displaying the name of the current Camera
    if (countNonZero(frames[curCamShowing]) >= 1){
      if(curCam == 0){
        imshow("Top", frames[curCamShowing]);
      }else if(curCam == 1){
        imshow("Up", frames[curCamShowing]);
      }else{
        imshow("Right", frames[curCamShowing]);
      }
    }else{
      std::cout<<curCamShowing<<std::endl;
    }
  }
  return 0;
}

int main(){
  CameraConfig hi;
  hi.build_camera_config();
}
