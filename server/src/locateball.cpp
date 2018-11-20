#include <iostream>
#include <time.h>
#include <thread>

#include "opencv2/opencv.hpp"
#include "opencv2/tracking.hpp"

#include "vector3.hpp"
#include "framebuffer.hpp"

#define ll long long

using namespace cv;

const int CAMERA_1 = 1; // cam 0 is the webcam
const int CAMERA_2 = 2;
const int CAMERA_3 = 3;
const int MULTITHREAD = 0;

FrameBuffer frameBuffer;


Mat detect_cones(Mat frame){
  Mat hsv;
  cvtColor(frame, hsv, CV_BGR2HSV);
  Scalar orange_lower(1,150,200);
  Scalar orange_upper(15,255,255);
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



bool process_frame(VideoCapture cap){
  if( waitKey(10) == 27 ) return 1; // stop capturing by pressing ESC
  Mat frame;
  cap.read(frame);
  GaussianBlur( frame, frame, Size(11, 11), 4, 4 );

  //note hsv range is from [0,179], [0,255], [0,255] (Hue, Saturation, Value)
  Mat hsv, hsv2;
  cvtColor(frame, hsv, CV_BGR2HSV);
  hsv.convertTo(hsv2, -1, 2, 0); //increase the contrast by the middle number
  //in dimmer environments the ball looks more orange and this is the mask for it
  Scalar purple_lower1(0,100,100); // I find that the ball isn't as affected by the middle number as other numbers. it is so high just to filter out the other colours
  Scalar purple_upper1(10,255,255); // limit the 255 if I don't want to orange cone. It's pretty bad cause they occupy almost the same hsv space
  //in brighter environments the ball looks more pink and this is the mask for it
  Scalar purple_lower2(105,80,80);
  Scalar purple_upper2(179,255,255);

  Mat purple_mask;
  Mat purple_mask1;
  Mat purple_mask2;
  inRange(hsv, purple_lower1, purple_upper1, purple_mask1);
  inRange(hsv, purple_lower2, purple_upper2, purple_mask2);

  addWeighted(purple_mask1, 1.0, purple_mask2, 1.0, 0.0, purple_mask);

  Mat final_image;
  purple_mask.convertTo(final_image, -1, 4, 0); //increase the contrast by the middle number
  Mat coneMask = detect_cones(frame);
  dilate(coneMask, coneMask, Mat(), Point(-1, -1), 2, 1, 1); 
  dilate(coneMask, coneMask, Mat(), Point(-1, -1), 2, 1, 1); 
  dilate(coneMask, coneMask, Mat(), Point(-1, -1), 2, 1, 1); 
  coneMask = ~coneMask;
  //imshow("asdsa",final_image);
  final_image = final_image & coneMask;
  final_image = findBiggestBlob(final_image);
  //bitwise_and( final_image, coneMask, final_image);
  //std::cout<<"asdsad"<<std::endl;

  //find the center of mass of the bitmas image
  Moments m = moments(final_image, false);
  Point p1(m.m10/m.m00, m.m01/m.m00);

  int x = Mat(p1).at<int>(0,0);
  int y = Mat(p1).at<int>(0,1);
  if(x != -2147483648){
    circle(frame, p1, 5, Scalar(128,0,0), -1); // plot the point of the ball
  }

  imshow("asdsa",frame);

  return 0;
}

int main(int argc, char** argv){
  VideoCapture cap;
  if(!cap.open(CAMERA_1))
    return 0;

  cap.set(CV_CAP_PROP_FRAME_WIDTH,640);
  cap.set(CV_CAP_PROP_FRAME_HEIGHT,480);
  cap.set(CV_CAP_PROP_AUTOFOCUS, 0);
  cap.set(CV_CAP_PROP_EXPOSURE, 0);

  Mat frame;
  cap.read(frame);
  while(true){
    if(process_frame(cap)){
      break;
    }
  }
  return 0;
}
