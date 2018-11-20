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

  cv::VideoCapture capArr[4];
  for(int i = 0 ; i < 4; i++){
    capArr[i].open(i);

    capArr[i].set(CV_CAP_PROP_FRAME_WIDTH,640);
    capArr[i].set(CV_CAP_PROP_FRAME_HEIGHT,480);
    capArr[i].set(CV_CAP_PROP_AUTOFOCUS, 0);
    capArr[i].set(CV_CAP_PROP_EXPOSURE, 0);
  }


  int curCam = 0;
  int camNumToIdx = -1;
  cv::Mat frames[4];
  int curCamShowing = 1;
  std::cout<<"NOTE: If this crashes at a specific cam move it to a diff port"<<std::endl;
  while(curCam < 3){

    int key = cv::waitKey(10);
    if(key == 27) return 1; // stop capturing by pressing ES
    else if(key == 13){ // ENTER pressed, selecting camera for curCam

    } else if(key == 32){ // SPACE pressed, cycle through the cameras
      curCamShowing++;
      if(curCamShowing == 4) curCamShowing = 0;
    } else if(key != -1) std::cout<<key<<std::endl;

    capArr[curCamShowing].read(frames[curCamShowing]);

    // Displaying the name of the current Camera
      if(curCam == 0){
        cv::imshow("Top", frames[curCamShowing]);
      }else if(curCam == 1){
        cv::imshow("Up", frames[curCamShowing]);
      }else{
        cv::imshow("Right", frames[curCamShowing]);
      }
  }
  return 0;
}

int main(){
  CameraConfig hi;
  hi.build_camera_config();
}
