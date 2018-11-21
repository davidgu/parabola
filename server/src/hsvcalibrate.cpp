#include <iostream>
#include "opencv2/opencv.hpp"
#include "opencv2/tracking.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/highgui.hpp"
//#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;

int main( int argc, char** argv ) {
  VideoCapture cap(0); //capture the video from web cam

  if ( !cap.isOpened() ){  
    std::cout << "Cannot open the web cam" << std::endl;
    return -1;
 }

  cap.set(CV_CAP_PROP_FRAME_WIDTH,480);
  cap.set(CV_CAP_PROP_FRAME_HEIGHT,480);

  namedWindow("Control", WINDOW_NORMAL); //create a window called "Control"

  int iLowH = 0;
  int iHighH = 179;

  int iLowS = 0; 
  int iHighS = 255;

  int iLowV = 0;
  int iHighV = 255;

  //Create trackbars in "Control" window
  imshow("Control", Mat::zeros(1, 500, CV_8UC1));
  cvCreateTrackbar("LowH\n", "Control", &iLowH, 179); //Hue (0 - 179)
  cvCreateTrackbar("HighH\n", "Control", &iHighH, 179);

  cvCreateTrackbar("LowS\n", "Control", &iLowS, 255); //Saturation (0 - 255)
  cvCreateTrackbar("HighS\n", "Control", &iHighS, 255);

  cvCreateTrackbar("LowV\n", "Control", &iLowV, 255); //Value (0 - 255)
  cvCreateTrackbar("HighV\n", "Control", &iHighV, 255);

  while (true) {
    Mat imgOriginal;

    bool bSuccess = cap.read(imgOriginal); // read a new frame from video

    if (!bSuccess){ 
      std::cout << "Cannot read a frame from video stream" << std::endl;
      break;
    }

    Mat imgHSV;

    cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV

    Mat imgThresholded;

    inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image

    //morphological opening (remove small objects from the foreground)
    erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
    dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 

    //morphological closing (fill small holes in the foreground)
    dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 
    erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

    imshow("Thresholded Image", imgThresholded); //show the thresholded image
    imshow("Original", imgOriginal); //show the original image

    if (waitKey(1) == 27){ 
      std::cout << "esc key is pressed by user" << std::endl;
      break; 
    }
  }

  return 0;
}
