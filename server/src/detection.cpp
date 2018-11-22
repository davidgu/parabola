#include <iostream>
#include <time.h>
#include <thread>

#include "opencv2/opencv.hpp"
#include "opencv2/tracking.hpp"

#include "vector3.hpp"
#include "framebuffer.hpp"

#define ll long long

using namespace cv;

const int CAMERA_1 = 0; // cam 0 is the webcam
const int CAMERA_2 = 1;
const int CAMERA_3 = 2;
const int MULTITHREAD = 0;

const std::string trackerList[8] = {"BOOSTING", "MIL", "KCF", "TLD","MEDIANFLOW", "GOTURN", "MOSSE", "CSRT"};
const int trackerSelection = 4;

FrameBuffer frameBuffer;


Mat detect_cones(Mat frame){
  Mat hsv;
  cvtColor(frame, hsv, CV_BGR2HSV);
  Scalar orange_lower(1,150,200); 
  Scalar orange_upper(15,255,255);
  Mat orange_mask;
  inRange(hsv, orange_lower, orange_upper,orange_mask);
  Mat detected_edges;
  blur( orange_mask, detected_edges, Size(3,3) );
  double lowThreshold = 14.0;
  double ratio = 3.0;
  int kernel_size = 3;
  Canny( detected_edges, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size );
  dilate(detected_edges, detected_edges, 1);
  erode(detected_edges, detected_edges, 1);
  Mat dst;
  dst = Scalar::all(0);
  frame.copyTo( dst, detected_edges);
  return dst;
}


void kmeansMask(const Mat& src, const Mat& mask, const int k, Mat& dst) {
  Mat tmp = Mat::zeros(mask.rows * mask.cols, 1, CV_8UC3);
  int counter = 0;
  for (int r = 0; r < mask.rows; ++r)
  {
    for (int c = 0; c < mask.cols; ++c)
    {
      if (mask.at<unsigned char>(r, c))
      {
        tmp.at<cv::Vec3f>(counter++, 0) = src.at<cv::Vec3b>(r, c);
      }
    }
  }

  Mat labels;
  kmeans(tmp, k, labels, TermCriteria(), 1, KMEANS_RANDOM_CENTERS);

  dst = cv::Mat(mask.size(), CV_32S, k);
  counter = 0;
  for (int r = 0; r < mask.rows; ++r) {
    for (int c = 0; c < mask.cols; ++c) {
      if (mask.at<unsigned char>(r, c)) {
        dst.at<int>(r, c) = labels.at<int>(counter++, 0);
      }
    }
  }
}

bool cone_detect_test(VideoCapture cap){
  if( waitKey(10) == 27 ) return 1; // stop capturing by pressing ESC
  Mat frame;
  cap.read(frame);
  frame = detect_cones(frame);
  Mat coneFrame = detect_cones(frame);
  Mat centers;
  kmeansMask(frame,coneFrame,4,centers);

  /*
     Mat samples(coneFrame.rows * coneFrame.cols, 1, CV_32FC2);
     for( int y = 0; y < coneFrame.rows; y++ ){
     for( int x = 0; x < coneFrame.cols; x++ ){
     samples.at<int>(y + x*coneFrame.rows) = coneFrame.at<int>(y,x);
     }
     }*/

  /*
  cv::Mat tmp = cv::Mat::zeros(cv::countNonZero(mask), 1, labImage.type());
  int counter = 0;
  for (int r = 0; r < mask.rows; ++r){
    for (int c = 0; c < mask.cols; ++c){
      if (!mask.at<unsigned char>(r, c)){
        // I assume Lab pixels are stored as a vector of floats
        tmp.at<cv::Vec3f>(counter++, 0) = labImage.at<cv::Vec3b>(r, c);
      }
    }
  }
  int clusterCount = 4;
  Mat labels;
  int attempts = 5;
  Mat centers;
  kmeans(tmp, clusterCount, labels, TermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 10000, 0.0001), attempts, KMEANS_PP_CENTERS, centers );
*/
  for (int i=0; i<centers.cols; i++) {
    circle(frame,Point(centers.at<int>(i,0),centers.at<int>(i,1)),4,Scalar(200));
  }
  /*
     Mat new_image( coneFrame.size(), coneFrame.type() );
     for( int y = 0; y < coneFrame.rows; y++ ){
     for( int x = 0; x < coneFrame.cols; x++ ) {
     int cluster_idx = labels.at<int>(y + x*coneFrame.rows,0);
// this is a color image
Point ipt = points.at<Point2f>(i);
circle( new_image, ipt, 2, colorTab[clusterIdx], FILLED, LINE_AA );
new_image.at<Vec3b>(y,x)[0] = centers.at<float>(cluster_idx, 0);
//new_image.at<Vec3b>(y,x)[1] = centers.at<float>(cluster_idx, 1);
//new_image.at<Vec3b>(y,x)[2] = centers.at<float>(cluster_idx, 2);
}
}*/

/*
   for(int i = 0; i < sampleCount; i++ ) {
   int clusterIdx = labels.at<int>(i);
   Point ipt = points.at<Point2f>(i);
   circle( new_image, ipt, 2, colorTab[clusterIdx], FILLED, LINE_AA );
   }*/
imshow( "clustered image", frame);
return 0;
}


bool custom_process_frame(VideoCapture cap){
  if( waitKey(10) == 27 ) return 1; // stop capturing by pressing ESC
  Mat frame;
  cap.read(frame);
  GaussianBlur( frame, frame, Size(11, 11), 4, 4);

  //note hsv range is from [0,179], [0,255], [0,255] (Hue, Saturation, Value)
  Mat hsv, hsv2;
  cvtColor(frame, hsv, CV_BGR2HSV);
  hsv.convertTo(hsv2, -1, 2, 0); //increase the contrast by the middle number
  //in dimmer environments the ball looks more orange and this is the mask for it
  Scalar purple_lower1(0,100,100); // I find that the ball isn't as affected by the middle number as other numbers. it is so high just to filter out the other colours
  Scalar purple_upper1(10,255,255); // limit the 255 if I don't want to orange cone. It's pretty bad cause they occupy almost the same hsv space

  Scalar purple_lower2(105,80,80);
  Scalar purple_upper2(179,255,255);

  //in brighter environments the ball looks more pink and this is the mask for it
  Mat purple_mask;
  Mat purple_mask1;
  Mat purple_mask2;
  inRange(hsv, purple_lower1, purple_upper1, purple_mask1);
  inRange(hsv, purple_lower2, purple_upper2, purple_mask2);

  //   erode(purple_mask1, purple_mask1,Mat(), Point(-1, -1), 2, 1, 1);
  //   erode(purple_mask1, purple_mask1,Mat(), Point(-1, -1), 2, 1, 1);
  //   dilate(purple_mask1, purple_mask1, Mat(), Point(-1, -1), 2, 1, 1);
  //   dilate(purple_mask1, purple_mask1, Mat(), Point(-1, -1), 2, 1, 1);


  //   erode(purple_mask2, purple_mask2,Mat(), Point(-1, -1), 2, 1, 1);
  //   erode(purple_mask2, purple_mask2,Mat(), Point(-1, -1), 2, 1, 1);
  //   dilate(purple_mask2, purple_mask2, Mat(), Point(-1, -1), 2, 1, 1);
  //   dilate(purple_mask2, purple_mask2, Mat(), Point(-1, -1), 2, 1, 1);

  addWeighted(purple_mask1, 1.0, purple_mask2, 1.0, 0.0, purple_mask);

  Mat final_image;
  purple_mask.convertTo(final_image, -1, 4, 0); //increase the contrast by the middle number

  //remove the cones from the mask
  Mat coneMask = detect_cones(frame);
  //imshow("asd",coneMask);

  dilate(coneMask, coneMask, Mat(), Point(-1, -1), 2, 1, 1);
  std::vector<std::pair<int,int> > cone_locations;// = get_cone_locations(coneMask);
  std::vector<Point> cone_points;
  for(int i = 0; i < cone_locations.size();i++){
    Point cur_point(cone_locations[i].first,cone_locations[i].second);
    cone_points.push_back(cur_point);
  }

  dilate(coneMask, coneMask, Mat(), Point(-1, -1), 2, 1, 1);
  dilate(coneMask, coneMask, Mat(), Point(-1, -1), 2, 1, 1);
  //we don't want to fill in the contours here because it might cover up the ball
  coneMask = ~coneMask;
  final_image = final_image & coneMask;

  //start analyzing the image
  std::vector<Vec3f> circles;

  Mat channels[3];
  split(frame,channels);  // planes[2] is the red channel


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

  // https://stackoverflow.com/questions/16746473/opencv-find-bounding-box-of-largest-blob-in-binary-image
  // https://www.learnopencv.com/blob-detection-using-opencv-python-c
  circle(frame, p1, 5, Scalar(128,0,0), -1); // plot the point of the ball
  for(int i = 0 ; i < cone_points.size();i++){
    circle(frame,cone_points[i],5,Scalar(128,128,128),-1);
  }

  // imshow("asdsa",frame);

  return 0;
}

int main(int argc, char** argv){
  VideoCapture cap;
  if(!cap.open(0))
    return 0;

  cap.set(CV_CAP_PROP_FRAME_WIDTH,640);
  cap.set(CV_CAP_PROP_FRAME_HEIGHT,480);
  cap.set(CV_CAP_PROP_AUTOFOCUS, 0);
  cap.set(CV_CAP_PROP_EXPOSURE, 0);

  // If multithread is enabled, each camera view will be read by an independent
  // thread, and processed data will be written to shared memory.

  if(MULTITHREAD){
  }else{
    Mat frame;
    cap.read(frame);
    while(true){
      if(custom_process_frame(cap)){
        break;
      }
    }
  }
  return 0;
}
