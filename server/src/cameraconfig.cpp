#include <string>
#include <fstream>
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

cv::Mat detect_cones(cv::Mat frame, int coneCali[2][3]){
  cv::Mat hsv;
  cv::cvtColor(frame, hsv, CV_BGR2HSV);
  cv::Scalar orange_lower(coneCali[0][0],coneCali[0][1],coneCali[0][2]);
  cv::Scalar orange_upper(coneCali[1][0],coneCali[1][1],coneCali[1][2]);
  cv::Mat orange_mask;
  cv::inRange(hsv, orange_lower, orange_upper,orange_mask);
  cv::Mat detected_edges;
  cv::blur( orange_mask, detected_edges, Size(3,3) );
  double lowThreshold = 14.0;
  double ratio = 3.0;
  int kernel_size = 3;
  cv::Canny( detected_edges, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size );
  cv::Mat dst;
  dst = cv::Scalar::all(0);
  frame.copyTo( dst, detected_edges);
  return dst;
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
  int calibratedCamIdx[3]; // Each index describes the camera number


  // I can put this in an array

  cv::VideoCapture capArr[4];
  for(int i = 0 ; i < 4; i++){
    if(!capArr[i].open(i)){
      std::cout<<"can't open cam: "<<i<<std::endl;
    }
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
      calibratedCamIdx[curCam] = curCamShowing;
      std::cout<<"cam "<<curCam<<" selected"<<std::endl;
      curCam++;
    } else if(key == 32){ // SPACE pressed, cycle through the cameras
      curCamShowing++;
      if(curCamShowing == 4) curCamShowing = 0;
    } else if(key != -1) std::cout<<key<<std::endl;

    capArr[curCamShowing].read(frames[curCamShowing]);


    // Displaying the name of the current Camera
    if(curCam == 0){
      putText(frames[curCamShowing], "Top", cv::Point2f(50,100), cv::FONT_HERSHEY_DUPLEX, 1,  cv::Scalar(0,0,255), 2);
    }else if(curCam == 1){
      putText(frames[curCamShowing], "Up", cv::Point2f(50,100), cv::FONT_HERSHEY_DUPLEX, 1,  cv::Scalar(0,0,255), 2);
    }else{
      putText(frames[curCamShowing], "Right", cv::Point2f(50,100), cv::FONT_HERSHEY_DUPLEX, 1,  cv::Scalar(0,0,255), 2);
    }
    cv::imshow("Camera", frames[curCamShowing]);
  }

  int rotatedCamIdx[3]; 
  curCam = 0;
  int curRot = 0;
  while(curCam < 3){
    int key = cv::waitKey(10);
    if(key == 27) return 1; // stop capturing by pressing ES
    else if(key == 13){ // ENTER pressed, selecting camera for curCam
      rotatedCamIdx[curCam] = curRot;
      std::cout<<"cam "<<curCam<<" has rotation: "<<curRot<<std::endl;

      // reset params
      curCam++;
      curRot = 0;
    } else if(key == 32){ // SPACE pressed, cycle through the cameras
      curRot += 90;
      if(curRot == 360) curRot = 0;
    } else if(key != -1) std::cout<<key<<std::endl;



    capArr[curCam].read(frames[curCam]);
    cv::Mat displayFrame;
    // cv::rotate(frames[curCam],displayFrame, curRot);

    cv::Point center = cv::Point( frames[curCam].cols/2, frames[curCam].rows/2 );
    double scale = 1.0;
    cv::Mat rot_mat = cv::getRotationMatrix2D( center, curRot, scale);
    warpAffine( frames[curCam], displayFrame, rot_mat, frames[curCam].size() );


    // Displaying the name of the current Camera
    if(curCam == 0){
      putText(frames[curCam], "Top", cv::Point2f(50,100), cv::FONT_HERSHEY_DUPLEX, 1,  cv::Scalar(0,0,255), 2);
    }else if(curCam == 1){
      putText(frames[curCam], "Up", cv::Point2f(50,100), cv::FONT_HERSHEY_DUPLEX, 1,  cv::Scalar(0,0,255), 2);
    }else{
      putText(frames[curCam], "Right", cv::Point2f(50,100), cv::FONT_HERSHEY_DUPLEX, 1,  cv::Scalar(0,0,255), 2);
    }
    cv::imshow("Camera", displayFrame);
  }
  std::cout<<"Top rot: "<<rotatedCamIdx[0]<<", Up rot: "<<rotatedCamIdx[1]<<", Right rot: "<<std::endl;

  // Using the top camera we need to calibrate the color:

  // Calibrate cones

  int coneCali[2][3] = {{1,150,200},{15,255,255}}; 
  int curRange = 0; // 0 = lower, 1 = upper

  // you control the hsv vals using... h: qw, s: er, v: ty
  while(1){
    int key = cv::waitKey(10);
    if(key == 27) return 1; // stop capturing by pressing ES
    else if(key == 13){ // ENTER pressed, selecting camera for curCam
      break;
    }else if(key == 113){ // decrease h 
      coneCali[curRange][0]--;
      coneCali[curRange][0] = std::max(0,coneCali[curRange][0]);
    }else if(key == 119){ // increase h 
      coneCali[curRange][0]++;
      coneCali[curRange][0] = std::min(179,coneCali[curRange][0]);
    } else if(key == 114){ // decrease s
      coneCali[curRange][1]--;
      coneCali[curRange][1] = std::max(0,coneCali[curRange][0]);
    }else if(key == 101){ // increase s
      coneCali[curRange][1]++;
      coneCali[curRange][1] = std::min(255,coneCali[curRange][0]);
    }else if(key == 121){ // decrease v
      coneCali[curRange][2]--;
      coneCali[curRange][2] = std::max(0,coneCali[curRange][0]);
    }else if(key == 116){ // increase v
      coneCali[curRange][2]++;
      coneCali[curRange][2] = std::min(255,coneCali[curRange][0]);
    }else if(key == 32){ // toggle range
      curRange = 1 - curRange;
    }

    if(key != -1){
      std::cout<<key<<std::endl;
    }
    capArr[1].read(frames[1]); // We'll use the top camera to do the calibration
    putText(frames[1], "h_l: " + std::to_string(coneCali[0][0]), cv::Point2f(5,50), cv::FONT_HERSHEY_DUPLEX, 1,  cv::Scalar(0,0,155), 2);
    putText(frames[1], "h_u: " + std::to_string(coneCali[1][0]), cv::Point2f(150,50), cv::FONT_HERSHEY_DUPLEX, 1,  cv::Scalar(0,0,155), 2);
    putText(frames[1], "s_l: " + std::to_string(coneCali[0][1]), cv::Point2f(5,100), cv::FONT_HERSHEY_DUPLEX, 1,  cv::Scalar(0,0,155), 2);
    putText(frames[1], "s_u: " + std::to_string(coneCali[1][1]), cv::Point2f(150,100), cv::FONT_HERSHEY_DUPLEX, 1,  cv::Scalar(0,0,155), 2);
    putText(frames[1], "v_l: " + std::to_string(coneCali[0][2]), cv::Point2f(5,150), cv::FONT_HERSHEY_DUPLEX, 1,  cv::Scalar(0,0,155), 2);
    putText(frames[1], "v_u: " + std::to_string(coneCali[1][2]), cv::Point2f(150,150), cv::FONT_HERSHEY_DUPLEX, 1,  cv::Scalar(0,0,155), 2);

    cv::Mat coneMast = detect_cones(frames,coneCali);
    cv::imshow("Camera", frames[1]);
    cv::imshow("Mask", coneMast);

  }


  // Calibrate Ball


  double topDist = 1.5;
  double upDist = 2.0;
  double rightDist = 2.0;


  // first line is the indexes of the camera
  // next we have the distance of the cameras to the middle
  // next is the rotation params for each camera (90 degrees)
  // next is the photo params for the colour of the cones
  // next is the photo params for the colour of the ball
  // next we have the locations of the cones from the top camera
  // next we have the flag for the dewarping of the photo
  // next is the rotation angle for the top camera

  /*std::ofstream myfile;
    myfile.open ("config.txt");
    myfile << ;
    myfile.close();*/
  return 0;
}

int main(){
  CameraConfig hi;
  hi.build_camera_config();
}
