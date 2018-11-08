#include <iostream>
#include <time.h>
#include <thread>

#include "opencv2/opencv.hpp"
#include "opencv2/tracking.hpp"

#include "vector3.h"
#include "framebuffer.h"

#define ll long long

using namespace cv;

const int CAMERA_1 = 0; // cam 0 is the webcam
const int CAMERA_2 = 1;
const int CAMERA_3 = 2;
const int MULTITHREAD = 0;

const std::string trackerList[8] = {"BOOSTING", "MIL", "KCF", "TLD","MEDIANFLOW", "GOTURN", "MOSSE", "CSRT"};
const int trackerSelection = 4;

FrameBuffer frameBuffer;

double get_fps(VideoCapture cap){
  int num_frames = 120;
  time_t start, end;
  Mat frame;

  // Time capturing specified number of frames
  time(&start);
  for(int i = 0; i < num_frames; i++){
    cap.read(frame);
  }
  time(&end);

  // Calculate FPS
  double seconds = difftime (end, start);
  return num_frames / seconds;
}

Ptr<Tracker> get_tracker(std::string trackerType){
  // Smart pointer to Tracker object
  Ptr<Tracker> tracker;
  if (trackerType == "BOOSTING")
    tracker = TrackerBoosting::create();
  if (trackerType == "MIL")
    tracker = TrackerMIL::create();
  if (trackerType == "KCF")
    tracker = TrackerKCF::create();
  if (trackerType == "TLD")
    tracker = TrackerTLD::create();
  if (trackerType == "MEDIANFLOW")
    tracker = TrackerMedianFlow::create();
  if (trackerType == "GOTURN")
    tracker = TrackerGOTURN::create();
  if (trackerType == "MOSSE")
    tracker = TrackerMOSSE::create();
  if (trackerType == "CSRT")
    tracker = TrackerCSRT::create();
  return tracker;
}

// Comparasion of speed between >> and .read
// .read is faster

long timediff(clock_t t1, clock_t t2) {
  long elapsed;
  elapsed = ((double)t2 - t1) / CLOCKS_PER_SEC * 1000;
  return elapsed;
}

void test_write_speed(VideoCapture cap){
  Mat stream_test;
  Mat read_test;

  clock_t start1, end1, start2, end2;
  start1 = clock();
  cap>>stream_test;
  end1 = clock();
  start2 = clock();
  cap.read(read_test);
  end2 = clock();
  std::printf("%ld\n",timediff(start1, end1));
  std::printf("%ld\n",timediff(start2, end2));
}

//std::vector<std::pair<int,int>> get_cone_locations(Mat frame){ 
// returns the locations of the <x,y> coords of n cones detected

//}

bool detect_cones(VideoCapture cap){
  if( waitKey(10) == 27 ) return 1; // stop capturing by pressing ESC
  //https://anikettatipamula.blogspot.com/2012/12/ball-tracking-detection-using-opencv.html
  //https://gist.github.com/razimgit/d9c91edfd1be6420f58a74e1837bde18
  //read input
  // idea: find the cones, draw the contours, then for each contour select the topmost point, this is the tip of the cone
  Mat frame;
  cap.read(frame);
  GaussianBlur( frame, frame, Size(11, 11), 4, 4 );


  Mat hsv;
  //note hsv range is from [0,179], [0,255], [0,255] (Hue, Saturation, Value)
  cvtColor(frame, hsv, CV_BGR2HSV);
  //hsv.convertTo(hsv, -1, 2, 0); //increase the contrast by the middle number
  Scalar orange_lower(1,150,200); // try 5 and 10 for the first index . that works pretty well // 2 200, 150
  Scalar orange_upper(15,255,255);

  //Scalar purple_lower(5,200,200);
  //Scalar purple_upper(30,255,255);

  Mat orange_mask;
  inRange(hsv, orange_lower, orange_upper,orange_mask);


  Mat final_image;
  //orange_mask.convertTo(final_image, -1, 2, 0); //increase the contrast by the middle number
  Mat channels[3];
  split(hsv,channels);
  imshow("Tracking",frame);


  /*
  //scale colour channels
  //this is bgr NOT rgb
  Mat channels[3];
  split(frame,channels);  // planes[2] is the red channel
  //channels[0]*=0.9; //blue
  //channels[1]*=0.5; //green
  //channels[2]*=0.6; //red
  Mat img;
  merge(channels,3, img);

  //mess with contrast
  Mat imageContrastHigh4;
  frame.convertTo(imageContrastHigh4, -1, 2, 0); //increase the contrast by 4

  //mess with brightness
  //Mat brightness;
  //brightness = imageContrastHigh4 +  Scalar(-100, -100, -100);
  Mat output_img;
  //GaussianBlur(brightness,output_img, Size(9, 9), 2, 2);
  //imshow("Tracking",output_img);
  //inRange(brightness, Scalar(55, 200, 100), Scalar(255, 255, 255), hue_range);

*/


  //Mat hue_range;
  /*
  // Mat hue_image;
  //addWeighted(lower_hue_range, 1.0, upper_hue_range, 1.0, 0.0, hue_image);
  //GaussianBlur(red_hue_image, red_hue_image, Size(9, 9), 2, 2);
  */
  //imshow("Tracking", imageContrastHigh4);
  /* std::vector<Vec3f> circles;
     HoughCircles(red_hue_image, circles, CV_HOUGH_GRADIENT, 1, red_hue_image.rows/8, 100, 20, 0, 0);*/
  return 0;
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



bool custom_process_frame(VideoCapture cap){
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

  Scalar purple_lower2(105,100,100);
  Scalar purple_upper2(179,255,255);

  //in brighter environments the ball looks more pink and this is the mask for it
  Mat purple_mask;
  Mat purple_mask1;
  Mat purple_mask2;
  inRange(hsv, purple_lower1, purple_upper1, purple_mask1);
  inRange(hsv, purple_lower2, purple_upper2, purple_mask2);
  addWeighted(purple_mask1, 1.0, purple_mask2, 1.0, 0.0, purple_mask);

  Mat final_image;
  purple_mask.convertTo(final_image, -1, 4, 0); //increase the contrast by the middle number







  //remove the cones from the mask
  //coneMask = 1 - coneMask;
  //final_image = add(final_image,coneMask);






  //start analyzing the image
  final_image = findBiggestBlob(final_image);
  std::vector<Vec3f> circles;

  Mat channels[3];
  split(frame,channels);  // planes[2] is the red channel


  Mat gray;
  //gray = purple_mask;
  gray = final_image;
  // I might not want to do this cause it might mess up the points of the ball
  erode(gray, gray, Mat(), Point(-1, -1), 2, 1, 1);
  dilate(gray, gray, Mat(), Point(-1, -1), 2, 1, 1);
  //cvtColor(frame, gray, CV_BGR2YCrCb);
  //cvtColor(frame, gray, CV_BGR2GRAY);
  /*HoughCircles( gray, circles, CV_HOUGH_GRADIENT, 1,2, 50, 60, 0, 1000 );
    for( size_t i = 0; i < circles.size(); i++ )
    {
    std::cout<<circles.size()<<std::endl;
    Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
    int radius = cvRound(circles[i][2]);
  // circle center
  circle( gray, center, 3, Scalar(0,255,0), -1, 8, 0 );
  // circle outline
  circle( gray, center, radius, Scalar(0,0,255), 3, 8, 0 );
  }*/

  //find the center of mass of the bitmas image
  Moments m = moments(final_image, false);
  Point p1(m.m10/m.m00, m.m01/m.m00);

  //std::cout << Mat(p1).at<int>(0,1) << std::endl;

  int x = Mat(p1).at<int>(0,0);
  int y = Mat(p1).at<int>(0,1);
  if(x != -2147483648){
    //std::cout<<x<<std::endl;
    //the ball is in view do something
  }

  //https://stackoverflow.com/questions/16746473/opencv-find-bounding-box-of-largest-blob-in-binary-image
  //https://www.learnopencv.com/blob-detection-using-opencv-python-c
  circle(frame, p1, 5, Scalar(128,0,0), -1);
  //circle(frame, p1, 5, Scalar(128,0,0), -1);
  imshow("asdsa",frame);

  //imshow("Tracking",final_image);
  //imshow("difff", gray);
  return 0;
}

void process_frame(VideoCapture cap, Ptr<Tracker> tracker, Rect2d bbox){
  Mat frame;
  cap.read(frame);

  // If tracking is successful, draw the bounding box
  bool ok = tracker->update(frame, bbox);
  if(ok){
    rectangle(frame, bbox, Scalar( 255, 0, 0 ), 2, 1 );
    printf("x: %f y: %f",bbox.x, bbox.y);
    // TODO: Determine which dimensions each camera is responsible for watching
    // TODO: Determine how to resolve conflicts between cameras
    // TODO: Write data to shared framebuffer
  }
  else{
    std::cout << "Tracking failure!" << std::endl;
  }
  imshow("Tracking", frame);
}

int main(int argc, char** argv){
  VideoCapture cap;
  if(!cap.open(CAMERA_2))
    return 0;

  cap.set(CV_CAP_PROP_FRAME_WIDTH,640);
  cap.set(CV_CAP_PROP_FRAME_HEIGHT,480);
  cap.set(CV_CAP_PROP_AUTOFOCUS, 0);
  cap.set(CV_CAP_PROP_EXPOSURE, 0);

  Ptr<Tracker> tracker = get_tracker(trackerList[trackerSelection]);
  Rect2d bbox(287, 23, 86, 320); 

  // If multithread is enabled, each camera view will be read by an independent
  // thread, and processed data will be written to shared memory.
  if(MULTITHREAD){
    std::thread c1(process_frame,cap,tracker,bbox);
    std::thread c2(process_frame,cap,tracker,bbox);
    std::thread c3(process_frame,cap,tracker,bbox);

    // Read position data from shared FrameBuffer and prepare data to be sent
    // to AR client
    while(true){

    }
  }
  else{
    Mat frame;
    cap.read(frame);
    rectangle(frame, bbox, Scalar( 255, 0, 0 ), 2, 1 ); 
    tracker->init(frame, bbox);
    imshow("Tracking", frame);
    while(true){
      if(detect_cones(cap)){
        break;
      }
      /*
         if( waitKey(10) == 27 ) break; // stop capturing by pressing ESC
         cap.read(frame);

      // If tracking is successful, draw the bounding box
      bool ok = tracker->update(frame, bbox);
      if(ok){
      rectangle(frame, bbox, Scalar( 255, 0, 0 ), 2, 1 );
      printf("x: %f y: %f\n",bbox.x, bbox.y);
      }
      else{
      std::cout << "Tracking failure!" << std::endl;
      }
      imshow("Tracking", frame);*/
    }
  }
  return 0;
}
