#include <iostream>
#include <time.h>
#include <thread>

#include "opencv2/opencv.hpp"
#include "opencv2/tracking.hpp"

#include "vector3.hpp"
#include "framebuffer.hpp"

#define ll long long

using namespace cv;


Mat detect_cones(Mat frame){
  //read input
  //   GaussianBlur( frame, frame, Size(11, 11), 4, 4 );
  Mat hsv;
  //note hsv range is from [0,179], [0,255], [0,255] (Hue, Saturation, Value)
  cvtColor(frame, hsv, CV_BGR2HSV);
  Scalar orange_lower(1,180,210); // try 5 and 10 for the first index . that works pretty well // 2 200, 150
  Scalar orange_upper(15,255,255);
  Mat orange_mask;
  inRange(hsv, orange_lower, orange_upper,orange_mask);

  return orange_mask;
}

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



void custom_process_frame(Mat frame){

  //note hsv range is from [0,179], [0,255], [0,255] (Hue, Saturation, Value)
  //GaussianBlur( frame, frame, Size(11, 11), 4, 4 );
  //frame = frame +  Scalar(-150, -150, -150);
  //frame.convertTo(frame, -1, 1.5, 0); //increase the contrast by the middle number

  Mat final_image = detect_cones_day(frame);
  //remove the cones from the mask
  //Mat coneMask = detect_cones(frame);
  //imshow("asd",coneMask);

  //dilate(coneMask, coneMask, Mat(), Point(-1, -1), 2, 1, 1);
  //dilate(coneMask, coneMask, Mat(), Point(-1, -1), 2, 1, 1);
  //dilate(coneMask, coneMask, Mat(), Point(-1, -1), 2, 1, 1);
  //we don't want to fill in the contours here because it might cover up the ball
  //coneMask = ~coneMask;
  //final_image = final_image & coneMask;

  //start analyzing the image
  final_image = findBiggestBlob(final_image);
  std::vector<Vec3f> circles;

  Mat channels[3];
  split(frame,channels);  // planes[2] is the red channel


  //find the center of mass of the bitmas image
  Moments m = moments(final_image, false);
  Point p1(m.m10/m.m00, m.m01/m.m00);


  int x = Mat(p1).at<int>(0,0);
  int y = Mat(p1).at<int>(0,1);
  if(x != -2147483648){
    // std::cout << Mat(p1).at<int>(0,1) << std::endl;
  }
  circle(frame, p1, 6, Scalar(256,256,256), -1); // plot the point of the ball

  imshow("asdsa",frame);

  return;
}


int main(int argc, char** argv){
  VideoCapture capArr[4];
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
  std::cout<<"Reading Frames"<<std::endl;
  int curCam = 3;
  int curRot[4] = {0};
  while(true){
    int key = waitKey(10);
    if(key == 13){ // ENTER pressed, select diff camera
      curCam++;
      if(curCam == 4) curCam = 0;
    }else if(key == 32){ // SPACE pressed, rotate cur cam
      curRot[curCam] += 90;
      if(curRot[curCam] == 360) curRot[curCam] = 0;
    }
    Mat frame;
    capArr[curCam].read(frame);
    cv::Point center = cv::Point( frame.cols/2, frame.rows/2 );
    double scale = 1.0;
    cv::Mat rot_mat = cv::getRotationMatrix2D( center, curRot[curCam], scale);
    warpAffine( frame, frame, rot_mat, frame.size() );
    custom_process_frame(frame);
  }
  return 0;
}
