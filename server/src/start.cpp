#include <fstream>
#include <thread>

#include "opencv2/opencv.hpp"

#include "vector3.hpp"
#include "vector2.hpp"
#include "trackedobject.hpp"
#include "vectorutils.hpp"
#include "simclock.hpp"

const bool THREE_CAMERA = false;
const bool DAYTIME = false;

using namespace cv;

VideoCapture capArr[4];

double topDist = 1.5;
double upDist = 2.0;
double rightDist = 2.0;
double sideCamHeights = 1.4;

const double cam_pfocallength = 466.18874961590114;

// 0 -> Top camera
// 1 -> North camera
// 2 -> East camera
Vector3 camWPoss[3];

int idxToCam[4];
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

  //infile>>idxToCam[0]>>idxToCam[1]>>idxToCam[2];
  infile>>rotCamIdx[0]>>rotCamIdx[1]>>rotCamIdx[2];
  infile>>coneCali[0][0]>>coneCali[0][1]>>coneCali[0][2]>>coneCali[1][0]>>coneCali[1][1]>>coneCali[1][2];
  infile>>ballCali1[0][0]>>ballCali1[0][1]>>ballCali1[0][2]>>ballCali1[1][0]>>ballCali1[1][1]>>ballCali1[1][2];
  infile>>ballCali2[0][0]>>ballCali2[0][1]>>ballCali2[0][2]>>ballCali2[1][0]>>ballCali2[1][1]>>ballCali2[1][2];
  infile>>topRotTheta;
}

Mat rotateFrame(Mat curFrame, int curCam){
    Point center = cv::Point( curFrame.cols/2, curFrame.rows/2 );
    double scale = 1.0;
    std::cout<<"rotation "<<curCam<<" "<<rotCamIdx[curCam]<<std::endl;
    Mat rot_mat = cv::getRotationMatrix2D( center, rotCamIdx[curCam], scale);
    Mat res;
    warpAffine( curFrame, res, rot_mat, curFrame.size() );
    return res;
}

void configure_cameras(){
  Mat frame;

  // internal camera index to opencv camera index
  for(int i = 0; i<4; i++){
    if(!capArr[i].open(i)){
      std::cout<<"can't open cam: "<<i<<std::endl;
    }
    capArr[i].set(CV_CAP_PROP_FRAME_WIDTH,640);
    capArr[i].set(CV_CAP_PROP_FRAME_HEIGHT,480);
    capArr[i].set(CV_CAP_PROP_AUTOFOCUS, 0);
    capArr[i].set(CV_CAP_PROP_EXPOSURE, 0);
    while(true){
      capArr[i].read(frame);
      putText(frame, "Please select the cam ID", cv::Point2f(50,100), cv::FONT_HERSHEY_DUPLEX, 1,  cv::Scalar(0,0,255), 2);
      int key = cv::waitKey(10);
      //if(key !=-1) std::cout<<key<<std::endl;
      // Zero key - This is always Top 
      if(key == 48){
        idxToCam[0] = i;
        break;
      }

      // One key - This is always North
      if(key == 49){
        idxToCam[1] = i;
        break;
      }

      // Two key - This is always East
      if(key == 50){
        idxToCam[2] = i;
        break;
      }

      // Three key - This should always be the webcam
      if(key == 51){
        idxToCam[3] = i;
        break;
      }
      imshow("cam", frame);
    }
  }
  destroyWindow("cam");
  // Waitkey must be here to trigger window destroy
  cv::waitKey(1);
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
  Scalar orange_lower(1,180,210); // try 5 and 10 for the first index . that works pretty well // 2 200, 150
  Scalar orange_upper(15,255,255);
  Mat orange_mask;
  inRange(hsv, orange_lower, orange_upper,orange_mask);
  return orange_mask;
}

// Detect cones function optimized for the daytime
Mat detect_cones_day(Mat frame){
  GaussianBlur( frame, frame, Size(9, 9), 4, 4 );
  frame = frame +  Scalar(-130, -130, -130);

  Mat hsv;
  cvtColor(frame, hsv, CV_BGR2HSV);
  Scalar orange_lower1(0,50,50);
  Scalar orange_upper1(10,255,255);

  Scalar orange_lower2(170,30,30);
  Scalar orange_upper2(179,255,255);
  Mat orange_mask;
  inRange(hsv, orange_lower1, orange_upper1,orange_mask);
  erode(orange_mask, orange_mask, Mat(), Point(-1, -1), 2, 1, 1);

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
  Mat final_image;
  if(DAYTIME){
    final_image = detect_cones_day(frame);
  }
  else{
    final_image = detect_cones_day(frame);
  }

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

  circle(frame, p1, 6, Scalar(256,256,256), -1); // plot the point of the ball


  // you can make it return a vector 2 of the location of the ball:
  return Vector2(x,y);
}

Vector2 cam_read_frame_pos(int idx, bool* success){
  Vector2 ballLoc;
  capArr[idxToCam[idx]].read(frames[idx]);
  //frames[idx] = rotateFrame(frames[idx],idx);
  bool detectSuccess = false;
  ballLoc = detect_ball(frames[idx], &detectSuccess);
  if(detectSuccess){
    *success = true;
    //std::cout<<"Camera "<<idx<<" DETECT"<<std::endl;
  }
  else{
    *success = false;
    //std::cout<<"Camera "<<idx<<" NO DETECT"<<std::endl;
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
  configure_cameras(); 
  /*
  for(int i = 0 ; i < 4; i++){
    if(!capArr[i].open(i)){
      std::cout<<"can't open cam: "<<i<<std::endl;
    }
    capArr[i].set(CV_CAP_PROP_FRAME_WIDTH,640);
    capArr[i].set(CV_CAP_PROP_FRAME_HEIGHT,480);
    capArr[i].set(CV_CAP_PROP_AUTOFOCUS, 0);
    capArr[i].set(CV_CAP_PROP_EXPOSURE, 0);
  }*/

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
        lines[i] = build_vector(i, cam_pfocallength, camWPoss[i], bpos);
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
      for(int i = 1; i < 3; i ++){
        Vector2 bpos = cam_read_frame_pos(i, &success);
        // If detection fails for one of the cameras, skip this set
        if(!success){
          break;
        }
        lines[i] = build_vector(i, cam_pfocallength, camWPoss[i], bpos);
        //std::cout << "Camera "<<i<<" ~ point: "<<lines[i].first<<", vec: "<<lines[i].second<<std::endl;
      }

      // If all cameras sucessfully detected the ball
      if(success){
        Vector3 ball_pred_pos = find_intersection(lines[1], lines[2]);
        tobject.add_pos(simClock.get_abstime(), ball_pred_pos);
        std::cout << ball_pred_pos << std::endl;
      }
    }
  }
}
