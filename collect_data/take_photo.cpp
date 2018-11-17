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
// 
// bool custom_process_frame(VideoCapture cap){
//   if( waitKey(10) == 27 ) return 1; // stop capturing by pressing ESC
//   Mat frame;
//   cap.read(frame);
//   GaussianBlur( frame, frame, Size(11, 11), 4, 4 );
// 
//   //note hsv range is from [0,179], [0,255], [0,255] (Hue, Saturation, Value)
//   Mat hsv, hsv2;
//   cvtColor(frame, hsv, CV_BGR2HSV);
//   hsv.convertTo(hsv2, -1, 2, 0); //increase the contrast by the middle number
//   //in dimmer environments the ball looks more orange and this is the mask for it
//   Scalar purple_lower1(0,100,100); // I find that the ball isn't as affected by the middle number as other numbers. it is so high just to filter out the other colours
//   Scalar purple_upper1(10,255,255); // limit the 255 if I don't want to orange cone. It's pretty bad cause they occupy almost the same hsv space
// 
//   Scalar purple_lower2(105,80,80);
//   Scalar purple_upper2(179,255,255);
// 
//   //in brighter environments the ball looks more pink and this is the mask for it
//   Mat purple_mask;
//   Mat purple_mask1;
//   Mat purple_mask2;
//   inRange(hsv, purple_lower1, purple_upper1, purple_mask1);
//   inRange(hsv, purple_lower2, purple_upper2, purple_mask2);
// 
//   //   erode(purple_mask1, purple_mask1,Mat(), Point(-1, -1), 2, 1, 1);
//   //   erode(purple_mask1, purple_mask1,Mat(), Point(-1, -1), 2, 1, 1);
//   //   dilate(purple_mask1, purple_mask1, Mat(), Point(-1, -1), 2, 1, 1);
//   //   dilate(purple_mask1, purple_mask1, Mat(), Point(-1, -1), 2, 1, 1);
// 
// 
//   //   erode(purple_mask2, purple_mask2,Mat(), Point(-1, -1), 2, 1, 1);
//   //   erode(purple_mask2, purple_mask2,Mat(), Point(-1, -1), 2, 1, 1);
//   //   dilate(purple_mask2, purple_mask2, Mat(), Point(-1, -1), 2, 1, 1);
//   //   dilate(purple_mask2, purple_mask2, Mat(), Point(-1, -1), 2, 1, 1);
// 
//   addWeighted(purple_mask1, 1.0, purple_mask2, 1.0, 0.0, purple_mask);
// 
//   Mat final_image;
//   purple_mask.convertTo(final_image, -1, 4, 0); //increase the contrast by the middle number
// 
//   //remove the cones from the mask
//   Mat coneMask = detect_cones(frame);
//   //imshow("asd",coneMask);
// 
//   dilate(coneMask, coneMask, Mat(), Point(-1, -1), 2, 1, 1);
//   std::vector<std::pair<int,int> > cone_locations = get_cone_locations(coneMask);
//   std::vector<Point> cone_points;
//   for(int i = 0; i < cone_locations.size();i++){
//     Point cur_point(cone_locations[i].first,cone_locations[i].second);
//     cone_points.push_back(cur_point);
//   }
// 
//   dilate(coneMask, coneMask, Mat(), Point(-1, -1), 2, 1, 1);
//   dilate(coneMask, coneMask, Mat(), Point(-1, -1), 2, 1, 1);
//   //we don't want to fill in the contours here because it might cover up the ball
//   coneMask = ~coneMask;
//   final_image = final_image & coneMask;
// 
//   //start analyzing the image
//   final_image = findBiggestBlob(final_image);
//   std::vector<Vec3f> circles;
// 
//   Mat channels[3];
//   split(frame,channels);  // planes[2] is the red channel
// 
// 
//   //find the center of mass of the bitmas image
//   Moments m = moments(final_image, false);
//   Point p1(m.m10/m.m00, m.m01/m.m00);
// 
//   //std::cout << Mat(p1).at<int>(0,1) << std::endl;
// 
//   int x = Mat(p1).at<int>(0,0);
//   int y = Mat(p1).at<int>(0,1);
//   if(x != -2147483648){
//     //std::cout<<x<<std::endl;
//     //the ball is in view do something
//   }
// 
//   //https://stackoverflow.com/questions/16746473/opencv-find-bounding-box-of-largest-blob-in-binary-image
//   //https://www.learnopencv.com/blob-detection-using-opencv-python-c
//   circle(frame, p1, 5, Scalar(128,0,0), -1); // plot the point of the ball
//   for(int i = 0 ; i < cone_points.size();i++){
//     circle(frame,cone_points[i],5,Scalar(128,128,128),-1);
//   }
// 
//   //   imshow("asdsa",frame);
// 
//   return 0;
// }

int main(int argc, char** argv){
  VideoCapture cap;
  if(!cap.open(CAMERA_2))
    return 0;
  cap.set(CV_CAP_PROP_FRAME_WIDTH,640);
  cap.set(CV_CAP_PROP_FRAME_HEIGHT,480);
  cap.set(CV_CAP_PROP_AUTOFOCUS, 0);
  cap.set(CV_CAP_PROP_EXPOSURE, 0);
  } else{
    Mat frame;
    cap.read(frame);
    imwrite("photos/" + std::to_string(clock()) +".jpg", frame);
  }
  return 0;
}
