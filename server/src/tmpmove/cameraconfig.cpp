#include <string>
#include <fstream>
#include "cameraconfig.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/tracking.hpp"
#include "vector3.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

// Initialize static class member
void *CameraConfig::currentlyEditingConfig = 0;

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

cv::Mat CameraConfig::detect_cones(cv::Mat frame, int coneCali[2][3]){
  cv::Mat hsv;
  cv::cvtColor(frame, hsv, CV_BGR2HSV);
  cv::Scalar orange_lower(coneCali[0][0],coneCali[0][1],coneCali[0][2]);
  cv::Scalar orange_upper(coneCali[1][0],coneCali[1][1],coneCali[1][2]);
  cv::Mat orange_mask;
  cv::inRange(hsv, orange_lower, orange_upper,orange_mask);
  cv::Mat detected_edges;
  cv::blur( orange_mask, detected_edges, cv::Size(3,3) );
  double lowThreshold = 14.0;
  double ratio = 3.0;
  int kernel_size = 3;
  cv::Canny( detected_edges, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size );
  cv::Mat dst;
  dst = cv::Scalar::all(0);
  frame.copyTo( dst, detected_edges);
  return dst;
}

void CameraConfig::match_cams(){
  // Note: the cameras in capArr must be opened
  int curCam = 0;
  int camNumToIdx = -1;
  int curCamShowing = 1;
  std::cout<<"NOTE: If this crashes at a specific cam move it to a diff port"<<std::endl;
  while(curCam < 3){
    int key = cv::waitKey(10);
    if(key == 27) return; // stop capturing by pressing ES
    else if(key == 13){ // ENTER pressed, selecting camera for curCam
      camIdx[curCam] = curCamShowing;
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
      putText(frames[curCamShowing], "North", cv::Point2f(50,100), cv::FONT_HERSHEY_DUPLEX, 1,  cv::Scalar(0,0,255), 2);
    }else{
      putText(frames[curCamShowing], "East", cv::Point2f(50,100), cv::FONT_HERSHEY_DUPLEX, 1,  cv::Scalar(0,0,255), 2);
    }
    cv::imshow("Camera", frames[curCamShowing]);
  }
}
void CameraConfig::fix_cam_rot(){
  // Note: the cameras in capArr must be opened
  int curCam = 0;
  int curRot = 0;

  while(curCam < 3){
    int key = cv::waitKey(10);
    if(key == 27) return; // stop capturing by pressing ES
    else if(key == 13){ // ENTER pressed, selecting camera for curCam
      rotCamIdx[curCam] = curRot;
      std::cout<<"cam "<<curCam<<" has rotation: "<<curRot<<std::endl;

      // reset params
      curCam++;
      curRot = 0;
      if(curCam == 3) break;
    } else if(key == 32){ // SPACE pressed, cycle through the cameras
      curRot += 90;
      if(curRot == 360) curRot = 0;
    } else if(key != -1) std::cout<<key<<std::endl;



    capArr[camIdx[curCam]].read(frames[curCam]);
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
      putText(frames[curCam], "North", cv::Point2f(50,100), cv::FONT_HERSHEY_DUPLEX, 1,  cv::Scalar(0,0,255), 2);
    }else{
      putText(frames[curCam], "East", cv::Point2f(50,100), cv::FONT_HERSHEY_DUPLEX, 1,  cv::Scalar(0,0,255), 2);
    }
    cv::imshow("Camera", displayFrame);
  }
  std::cout<<"Top rot: "<<rotCamIdx[0]<<", North rot: "<<rotCamIdx[1]<<", East rot: "<<rotCamIdx[2]<<std::endl;
}

void CameraConfig::calibrate_cones(){
  // This can be refactored with calibrate balls later
  int curRange = 0; // 0 = lower, 1 = upper

  // you control the hsv vals using... h: qw, s: er, v: ty
  while(1){
    int key = cv::waitKey(1);
    if(key == 27) return; // stop capturing by pressing ES
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
    capArr[camIdx[1]].read(frames[1]); // We'll use the up camera to do the calibration
    cv::namedWindow("Camera", cv::WINDOW_AUTOSIZE);
    putText(frames[1], "h_l: " + std::to_string(coneCali[0][0]), cv::Point2f(5,50), cv::FONT_HERSHEY_DUPLEX, 1,  cv::Scalar(0,0,155), 2);
    putText(frames[1], "h_u: " + std::to_string(coneCali[1][0]), cv::Point2f(150,50), cv::FONT_HERSHEY_DUPLEX, 1,  cv::Scalar(0,0,155), 2);
    putText(frames[1], "s_l: " + std::to_string(coneCali[0][1]), cv::Point2f(5,100), cv::FONT_HERSHEY_DUPLEX, 1,  cv::Scalar(0,0,155), 2);
    putText(frames[1], "s_u: " + std::to_string(coneCali[1][1]), cv::Point2f(150,100), cv::FONT_HERSHEY_DUPLEX, 1,  cv::Scalar(0,0,155), 2);
    putText(frames[1], "v_l: " + std::to_string(coneCali[0][2]), cv::Point2f(5,150), cv::FONT_HERSHEY_DUPLEX, 1,  cv::Scalar(0,0,155), 2);
    putText(frames[1], "v_u: " + std::to_string(coneCali[1][2]), cv::Point2f(150,150), cv::FONT_HERSHEY_DUPLEX, 1,  cv::Scalar(0,0,155), 2);

    cv::Mat coneMask = detect_cones(frames[1],coneCali);
    cv::imshow("Camera", frames[1]);
    cv::imshow("Mask", coneMask);
  }
}

void CameraConfig::calibrate_ball(int maskId){

}

void detect_cones_helper(int event, int x, int y, int flags, void* param){
  cv::Mat mat = *((cv::Mat*)param);
  if(event == CV_EVENT_LBUTTONDOWN){
    CameraConfig *edCfg = (CameraConfig*)CameraConfig::currentlyEditingConfig;
    edCfg->coneLocations.push_back(Vector2(x,y));
    std::cout<<"Added cone at: "<<x<<" "<<y<<std::endl;
  }
}

void CameraConfig::set_currently_editing_cfg(CameraConfig *cfgptr){
  CameraConfig::currentlyEditingConfig = (void *)cfgptr;
}

void CameraConfig::detect_cones(){
CameraConfig::currentlyEditingConfig = this;
  while(1){
    int key = cv::waitKey(1);
    if(key == 13 || coneLocations.size() == 4){ // ENTER pressed
      break;
    }
    capArr[camIdx[0]].read(frames[0]); // We'll use the top camera to do the detection
      putText(frames[0], "Identify Cones", cv::Point2f(50,100), cv::FONT_HERSHEY_DUPLEX, 1,  cv::Scalar(0,0,255), 2);
    cv::namedWindow("Camera", cv::WINDOW_AUTOSIZE);
    cv::setMouseCallback("Camera", detect_cones_helper, (void*) &frames[0]);

    std::vector<cv::Point> conePoints;
    for(int i = 0; i < coneLocations.size();i++){
      cv::Point curPoint(coneLocations[i].x,coneLocations[i].y);
      conePoints.push_back(curPoint);
    }
    for(int i = 0 ; (unsigned) i < conePoints.size();i++){
      circle(frames[0],conePoints[i],5,cv::Scalar(128,128,128),-1);
    }
    cv::imshow("Camera", frames[0]);
  }
}

void CameraConfig::calc_top_rot_theta(){
  // given the locations of the cones, returns the ball's catesian location
  // Note: The camera must be within 90 degrees of facing in the right orientation
  assert(coneLocations.size() == 4);

  // calculate the midpoint of the cones, needed to find the origin
  Vector2 midpoint;
  for(int i = 0 ; (unsigned) i < coneLocations.size();i++){
    midpoint.x += coneLocations[i].x;
    midpoint.y += coneLocations[i].y;
  }
  midpoint.x/=4;
  midpoint.y/=4;


  // calculate which direction to rotate and how much
  double lowestY = 999999.0;
  double sLowestY = 999999.0;
  Vector2 lowestYPair;
  Vector2 sLowestYPair;

  // finds the lowest point
  for(int i = 0; (unsigned) i < coneLocations.size();i++){
    double x = coneLocations[i].x;
    double y = coneLocations[i].y;
    if( y < lowestY){
      // update the second lowest
      sLowestY = lowestY;
      sLowestYPair = lowestYPair;
      // update the lowest pair
      lowestY = y;
      lowestYPair = coneLocations[i];
    }else if(y < sLowestY){
      sLowestY = y;
      sLowestYPair = coneLocations[i];
    }
  }

  double xDiff = -1;
  double yDiff = -1;
  // std::cout<<lowestYPair.x<<" "<<lowestYPair.y<<std::endl;
  // std::cout<<sLowestYPair.x<<" "<<sLowestYPair.y<<std::endl;
  if(lowestYPair.x > sLowestYPair.x){ // The lower point it to the bottom right
    xDiff = lowestYPair.x - sLowestYPair.x;
    yDiff = sLowestYPair.y - lowestYPair.y;
  }else{
    xDiff = sLowestYPair.x - lowestYPair.x;
    yDiff = sLowestYPair.y - lowestYPair.y;
  }

  topRotTheta = atan(yDiff/xDiff);
  if(lowestYPair.x < sLowestYPair.x) topRotTheta=topRotTheta * -1.0;
}

int CameraConfig::build_camera_config(){
  // Cycles through the cameras to find which one it is
  // Also states how far the cameras are from the middle
  /*int numCams = countCameras();
    if(numCams != 4){
    std::cout<<"Four cameras need to be connected. The number of cameras are: "<<numCams<<std::endl;
    return;
    }*/
  //  std::cout<<"4 Cameras connected. Camera Location Detection Running."<<std::endl;

  // idx 0 = top, idx 1 = up, idx 2 = right

  for(int i = 0 ; i < 4; i++){
    if(!capArr[i].open(i)){
      std::cout<<"Can't open cam: "<<i<<std::endl;
    }
    capArr[i].set(CV_CAP_PROP_FRAME_WIDTH,640);
    capArr[i].set(CV_CAP_PROP_FRAME_HEIGHT,480);
    capArr[i].set(CV_CAP_PROP_AUTOFOCUS, 0);
    capArr[i].set(CV_CAP_PROP_EXPOSURE, 0);
  }
  CameraConfig::match_cams();
  CameraConfig::fix_cam_rot();

  // Using the top camera we need to calibrate the color:
  if(MANUALLY_MODIFY_MASKS){
    std::cout<<"Manually calibrating cones"<<std::endl;
    CameraConfig::calibrate_cones();

    std::cout<<"Manually calibrating ball"<<std::endl;
    CameraConfig::calibrate_ball(1);
    CameraConfig::calibrate_ball(2);
  }

  std::cout<<"Detecting Cones"<<std::endl;
  CameraConfig::detect_cones();
  std::cout<<"Calculating Rotation Theta for Top Camera"<<std::endl;
  CameraConfig::calc_top_rot_theta();


  // Why do we need to locations of the cones? We need to know how to dewarp

  // first line is the indexes of the camera
  // next we have the position vectors of the 3 cameras
  // next is the rotation params for each camera (90 degrees)
  // next is the photo params for the colour of the cones
  // next is the photo params for the colour of the ball1
  // next is the photo params for the colour of the ball2
  // next we have the locations of the cones from the top camera
  // next is the rotation angle for the top camera
  // next we have the flag for the dewarping of the photo (PLEASE IMPLEMENT)

  std::cout<<"Saving Config File"<<std::endl;
  std::ofstream myfile;
    myfile.open ("config.txt");
    myfile << topCameraHeight<<" "<<northCameraX<<" "<<northCameraY<<" "<<eastCameraZ<<" "<<eastCameraY<<std::endl;
    myfile << camIdx[0]<<" "<<camIdx[1]<<" "<<camIdx[2]<<std::endl;
    myfile << rotCamIdx[0]<<" "<<rotCamIdx[1]<<" "<<rotCamIdx[2]<<std::endl;
    myfile << coneCali[0][0]<<" "<<coneCali[0][1]<<" "<<coneCali[0][2]<<" "<<coneCali[1][0]<<" "<<coneCali[1][1]<<" "<<coneCali[1][2]<<std::endl;
    myfile << ballCali1[0][0]<<" "<<ballCali1[0][1]<<" "<<ballCali1[0][2]<<" "<<ballCali1[1][0]<<" "<<ballCali1[1][1]<<" "<<ballCali1[1][2]<<std::endl;
    myfile << ballCali2[0][0]<<" "<<ballCali2[0][1]<<" "<<ballCali2[0][2]<<" "<<ballCali2[1][0]<<" "<<ballCali2[1][1]<<" "<<ballCali2[1][2]<<std::endl;
    myfile << coneLocations[0].x<<" "<< coneLocations[0].y<<" "<<coneLocations[1].x<<" "<<coneLocations[1].y<<" "<<coneLocations[2].x<<" "<<coneLocations[2].y<<std::endl;
    myfile << topRotTheta<<std::endl;
    myfile.close();
  std::cout<<"Config File Saved!"<<std::endl;
  return 0;
}

int main(){
  CameraConfig hi;
  hi.build_camera_config();
}
