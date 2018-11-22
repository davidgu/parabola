#include <fstream>
#include <thread>

#include "opencv2/opencv.hpp"

#include "vector3.hpp"
#include "vector2.hpp"
#include "trackedobject.hpp"
#include "vectorutils.hpp"
#include "simclock.hpp"

const bool THREE_CAMERA = false;

using namespace cv;

VideoCapture capArr[4];

double topDist = 1.5;
double upDist = 2.0;
double rightDist = 2.0;
double sideCamHeights = 1.4;

// 0 -> Top camera
// 1 -> North camera
// 2 -> East camera
Vector3 camWPoss[3];

int camIdx[3];
int rotCamIdx[3];
int coneCali[2][3];
int ballCali1[2][3];
int ballCali2[2][3];
double topRotTheta;
Mat frames[3];

TrackedObject tobject;
SimClock simClock;

void read_config_file(){
  std::ifstream infile("config.txt");

  double topCameraHeight;
  infile>>topCameraHeight;
  camWPoss[0] = Vector3(0, topCameraHeight, 0);

  double northCameraX;
  double northCameraY;
  infile>> northCameraX;
  infile>> northCameraY;
  camWPoss[1] = Vector3(northCameraX, northCameraY, 0);

  double eastCameraZ;
  double eastCameraY;
  infile>> eastCameraZ;
  infile>> eastCameraY;
  camWPoss[2] = Vector3(0, eastCameraY, eastCameraZ);

  infile>>camIdx[0]>>camIdx[1]>>camIdx[2];
  infile>>rotCamIdx[0]>>rotCamIdx[1]>>rotCamIdx[2];
  infile>>coneCali[0][0]>>coneCali[0][1]>>coneCali[0][2]>>coneCali[1][0]>>coneCali[1][1]>>coneCali[1][2];
  infile>>ballCali1[0][0]>>ballCali1[0][1]>>ballCali1[0][2]>>ballCali1[1][0]>>ballCali1[1][1]>>ballCali1[1][2];
  infile>>ballCali2[0][0]>>ballCali2[0][1]>>ballCali2[0][2]>>ballCali2[1][0]>>ballCali2[1][1]>>ballCali2[1][2];
  infile>>topRotTheta;
}

Vector2 fix_top_angle(Vector2 ballLoc){
  Vector2 ans;
  // here we are multiplying the location vector by a rotation matrix
  ans.x = cos(topRotTheta)*ballLoc.x - sin(topRotTheta)*ballLoc.y;
  ans.y = sin(topRotTheta)*ballLoc.x + cos(topRotTheta)*ballLoc.y;
  return ans;
}

Mat detect_cones(Mat frame){
  Mat hsv;
  cvtColor(frame, hsv, CV_BGR2HSV);
  Scalar orange_lower(coneCali[0][0], coneCali[0][1], coneCali[0][2]); 
  Scalar orange_upper(coneCali[1][0], coneCali[1][1], coneCali[1][2]);
  Mat orange_mask;
  inRange(hsv, orange_lower, orange_upper,orange_mask);
  return orange_mask;
}

Mat findBiggestBlob(Mat & matImage){
  int largest_area=0;
  int largest_contour_index=0;

  std::vector< std::vector<Point> > contours; // Vector for storing contour
  std::vector<Vec4i> hierarchy;
  Mat newImg(matImage.size(), CV_64FC1);
  newImg = 0;

  findContours( matImage, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE ); // Find the contours in the image

  for( int i = 0; i< contours.size(); i++ ) {// iterate through each contour. 
    double a=contourArea( contours[i],false);  //  Find the area of contour
    if(a>largest_area){
      largest_area=a;
      largest_contour_index=i;                //Store the index of largest contour
      //bounding_rect=boundingRect(contours[i]); // Find the bounding rectangle for biggest contour
    }
  }

  drawContours( newImg, contours, largest_contour_index, Scalar(255), CV_FILLED, 8, hierarchy ); // Draw the largest contour using previously stored index.
  return newImg;
}

Vector2 detect_ball(Mat frame, bool *success){
  GaussianBlur( frame, frame, Size(11, 11), 4, 4);
  Mat final_image = detect_cones(frame);
  final_image = findBiggestBlob(final_image);

  //start analyzing the image
  std::vector<Vec3f> circles;
  // find the center of mass of the bitmask image
  Moments m = moments(final_image, false);
  Point p1(m.m10/m.m00, m.m01/m.m00);

  int x = Mat(p1).at<int>(0,0);
  int y = Mat(p1).at<int>(0,1);

  // If something is detected
  if(x != -2147483648){
    // Implement arduino serial communication
    *success = true;
  }
  else{
    *success = false;
  }

  // you can make it return a vector 2 of the location of the ball:
  return Vector2(x,y);
}

Vector2 cam_read_frame_pos(int idx, bool* success){
  Vector2 ballLoc;
  capArr[camIdx[idx]].read(frames[idx]);
  bool detectSuccess = false;
  ballLoc = detect_ball(frames[idx], &detectSuccess);
  if(detectSuccess){
    *success = true;
  }
  else{
    *success = false;
  }
  return ballLoc;
}

// Required for camera 0 ~ top camera
Vector2 correct_image_angle(Vector2 in){
  return fix_top_angle(in);
}

int main(){
  // read from config file and initialize params
  std::cout<<"Reading config file"<<std::endl;
  read_config_file();

  std::cout<<"Initializing Cameras"<<std::endl;
  for(int i = 0 ; i < 4; i++){
    if(!capArr[i].open(i)){
      std::cout<<"can't open cam: "<<i<<std::endl;
    }
    capArr[i].set(CV_CAP_PROP_FRAME_WIDTH,640);
    capArr[i].set(CV_CAP_PROP_FRAME_HEIGHT,480);
    capArr[i].set(CV_CAP_PROP_AUTOFOCUS, 0);
    capArr[i].set(CV_CAP_PROP_EXPOSURE, 0);
  }

  // Start simclock
  simClock = SimClock();

  std::cout<<"Reading Frames"<<std::endl;

  std::pair<Vector3, Vector3> lines[3];

  while(true){
    // Clear terminal
    //std::cout << "\033[2J\033[1;1H";
    if(THREE_CAMERA){
      clock_t start = clock();

      bool success = false;
      for(int i = 0; i < 3; i ++){
        Vector2 bpos = cam_read_frame_pos(i, &success);
        if(i == 0){
          bpos = correct_image_angle(bpos);
        }
        lines[i] = build_vector(i, camWPoss[i], bpos);
      }
      Vector3 ball_pred_pos = find_intersection(lines[0], lines[1], lines[2]);
      tobject.add_pos(simClock.get_abstime(), ball_pred_pos);

      clock_t end = clock();
      clock_t elapsed = end - start;

      //std::cout << ball_pred_pos << std::endl;
      //std::cout << "Time Elapsed: "<< elapsed << std::endl;
    }
    // This is the two camera case
    else{
      bool success = false;
      for(int i = 0; i < 2; i ++){
        Vector2 bpos = cam_read_frame_pos(i, &success);
        // If detection fails for one of the cameras, skip this set
        if(!success){
          break;
        }
        lines[i] = build_vector(i, camWPoss[i], bpos);
      }

      // If all cameras sucessfully detected the ball
      if(success){
        Vector3 ball_pred_pos = find_intersection(lines[0], lines[1]);
        tobject.add_pos(simClock.get_abstime(), ball_pred_pos);
        std::cout << ball_pred_pos << std::endl;
      }
    }
  }
}
