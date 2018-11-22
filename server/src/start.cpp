#include <string>
#include <fstream>
#include "cameraconfig.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/tracking.hpp"
#include "vector3.hpp"
#include "vector2.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

VideoCapture capArr[4];
double topDist = 1.5;
double upDist = 2.0;
double rightDist = 2.0;
double sideCamHeights = 1.4;
int camIdx[3];
int rotCamIdx[3];
int coneCali[2][3];
int ballCali1[2][3];
int ballCali2[2][3];
double topRotTheta;
Mat frames[3];

void read_file(){
  std::ifstream infile("config.txt");
  infile>>topDist>>upDist>>rightDist>>sideCamHeights;
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

  /*Mat detected_edges;
    blur( orange_mask, detected_edges, Size(3,3) );
    double lowThreshold = 14.0;
    double ratio = 3.0;
    int kernel_size = 3;
    Canny( detected_edges, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size );
    dilate(detected_edges, detected_edges, 1);
    erode(detected_edges, detected_edges, 1);
    Mat dst;
    dst = Scalar::all(0);
    frame.copyTo( dst, detected_edges);*/
  return orange_mask;
}

Mat findBiggestBlob(Mat & matImage){
  int largest_area=0;
  int largest_contour_index=0;

  std::vector< std::vector<Point> > contours; // Vector for storing contour
  std::vector<Vec4i> hierarchy;
  Mat newImg(matImage.size(), CV_64FC1);
  newImg = 0;

  findContours( matImage, contours, hierarchy,CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE ); // Find the contours in the image

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

Vector2 detect_ball(Mat frame){
  waitKey(10);
  GaussianBlur( frame, frame, Size(11, 11), 4, 4);

  Mat hsv, hsv2;
  cvtColor(frame, hsv, CV_BGR2HSV);
  hsv.convertTo(hsv2, -1, 2, 0); 
  Scalar purple_lower1(ballCali1[0][0], ballCali1[0][1], ballCali1[0][2]); 
  Scalar purple_upper1(ballCali1[1][0], ballCali1[1][1], ballCali1[1][2]);

  Scalar purple_lower2(ballCali2[0][0], ballCali2[0][1], ballCali2[0][2]); 
  Scalar purple_upper2(ballCali2[1][0], ballCali2[1][1], ballCali2[1][2]);

  Mat purple_mask;
  Mat purple_mask1;
  Mat purple_mask2;
  inRange(hsv, purple_lower1, purple_upper1, purple_mask1);
  inRange(hsv, purple_lower2, purple_upper2, purple_mask2);
  addWeighted(purple_mask1, 1.0, purple_mask2, 1.0, 0.0, purple_mask);


  Mat final_image;
  purple_mask.convertTo(final_image, -1, 4, 0); 

  //remove the cones from the mask
  Mat coneMask = detect_cones(frame);
  dilate(coneMask, coneMask, Mat(), Point(-1, -1), 2, 1, 1);
  dilate(coneMask, coneMask, Mat(), Point(-1, -1), 2, 1, 1);
  dilate(coneMask, coneMask, Mat(), Point(-1, -1), 2, 1, 1);
  //we don't want to fill in the contours here because it might cover up the ball
  coneMask = ~coneMask;
  final_image = final_image & coneMask;
  final_image = findBiggestBlob(final_image);

  //start analyzing the image
  std::vector<Vec3f> circles;
  // find the center of mass of the bitmask image
  Moments m = moments(final_image, false);
  Point p1(m.m10/m.m00, m.m01/m.m00);
  // std::cout << Mat(p1).at<int>(0,1) << std::endl;
  int x = Mat(p1).at<int>(0,0);
  int y = Mat(p1).at<int>(0,1);
  if(x != -2147483648){
    //std::cout<<x<<std::endl;
    //the ball is in view do something
  }
  circle(frame, p1, 5, Scalar(128,0,0), -1); // plot the point of the ball
  imshow("asdsa",frame);


  // you can make it return a vector 2 of the location of the ball:
  return Vector2(x,y);
}
int main(){
  // read from config file and initialize params
  read_file();

  // todo: do this for all 3 cameras
  Mat frame;


  for(int i = 0 ; i < 4; i++){
    if(!capArr[i].open(i)){
      std::cout<<"can't open cam: "<<i<<std::endl;
    }
    capArr[i].set(CV_CAP_PROP_FRAME_WIDTH,640);
    capArr[i].set(CV_CAP_PROP_FRAME_HEIGHT,480);
    capArr[i].set(CV_CAP_PROP_AUTOFOCUS, 0);
    capArr[i].set(CV_CAP_PROP_EXPOSURE, 0);
  }


  Vector2 ballLoc;

  while(true){
  // put these on separate threads
    for(int curCam = 0 ; curCam < 1 ; curCam++){
      capArr[camIdx[curCam]].read(frames[curCam]);

      ballLoc = detect_ball(frames[curCam]);
      if(curCam == 0){ // This cameras needs to rotation dewarp
        ballLoc = fix_top_angle(ballLoc);
      }
    }
  }

}
