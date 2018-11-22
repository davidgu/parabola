#include <iostream>

#include "opencv2/calib3d.hpp"
#include "opencv2/opencv.hpp"

using namespace cv;

// Recalibrate these camera intrinsics. These are not accurate.
const Mat cameraMatrix = (Mat1d(3, 3)<< 4.6618874961590114e+02, 0, 320, 0, 4.6618874961590114e+02, 213, 0, 0, 1);
const Mat distortionCoefficients = (Mat1d(1, 5)<< -7.6634604887242289e-02, 2.4678415075705742e-01, -5.9084432300489159e-03, 8.1790420843563837e-04, -2.9210824264458785e-01);

Mat projMat1, projMat2;

VideoCapture capArr[4];
int idxToCam[4];

Mat detect_cones(Mat frame){
  Mat hsv;
  cvtColor(frame, hsv, CV_BGR2HSV);
  Scalar orange_lower(1,180,210); // try 5 and 10 for the first index . that works pretty well // 2 200, 150
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

Point2f detect_ball(Mat frame, bool *success){
  GaussianBlur(frame, frame, Size(5, 5), 4, 4);
  //frame = frame + Scalar(-50, -50, -50);
  //frame.convertTo(frame, -1, 1.4, 0);
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
    circle(frame, p1, 6, Scalar(256,256,256), -1); // plot the point of the ball
    if(success != nullptr){
      *success = true;
    }
  }
  else{
    if(success != nullptr){
      *success = false;
    }
  }
  return Point2f(x,y);
}

void configure_cameras(){
  Mat frame;
  for(int i = 0; i<4; i++){
    if(!capArr[i].open(i)){
        std::cout<<"can't open cam: "<<i<<std::endl;
    }
    else{
        capArr[i].set(CV_CAP_PROP_FRAME_WIDTH,640);
        capArr[i].set(CV_CAP_PROP_FRAME_HEIGHT,480);
        capArr[i].set(CV_CAP_PROP_AUTOFOCUS, 0);
        capArr[i].set(CV_CAP_PROP_EXPOSURE, 0);
        while(true){
        capArr[i].read(frame);
        putText(frame, "Please select the cam ID", cv::Point2f(50,100), cv::FONT_HERSHEY_DUPLEX, 1,  cv::Scalar(0,0,255), 2);
        int key = cv::waitKey(10);
        // Zero key
        if(key == 48){
            idxToCam[0] = i;
            break;
        }
        // One key
        if(key == 49){
            idxToCam[1] = i;
            break;
        }
        // Two key
        if(key == 50){
            idxToCam[2] = i;
            break;
        }
        // Three key
        if(key == 51){
            idxToCam[3] = i;
            break;
        }
        imshow("cam", frame);
        }
    }
  }
  destroyWindow("cam");
  // Waitkey must be here to trigger window destroy
  cv::waitKey(1);
}

std::pair<Mat,Mat> capture_frames(VideoCapture cam1, VideoCapture cam2){
    Mat frame1, frame2;

    for(;;){
        cam1.read(frame1);
        cam2.read(frame2);

        // Make imshow work
        waitKey(1);

        // Detect ball in order to draw overlay if ball is being tracked
        detect_ball(frame1, nullptr);
        detect_ball(frame2, nullptr);

        imshow("Camera 1", frame1);
        imshow("Camera 2", frame2);

        int key = waitKey(1);

        // space bar
        if(key == 32){
            return std::pair<Mat, Mat>(frame1, frame2);
        }
    }
}



// Gets the projection matrices for the two cameras
void calibrate(VideoCapture cam1, VideoCapture cam2){
    Mat frames1[4];
    Mat frames2[4];
    Mat rvec1, rvec2, tvec1, tvec2;

    std::vector<Point3f> worldCoords = {
        Point3f(0,0,0),
        Point3f(0,0,1),
        Point3f(0,1,0),
        Point3f(1,0,0),
    };

    std::vector<Point2f> frame1Coords;
    std::vector<Point2f> frame2Coords;

    std::pair<Mat, Mat> fpair;
    std::cout<<"Coordinates will be expressed as (X, Y, Z)"<<std::endl;
    std::cout<<"Make sure the ball is visible by both cameras at all times."<<std::endl;
    std::cout<<"Bring the ball to (0, 0, 0) and press the space bar"<<std::endl;
    fpair = capture_frames(cam1, cam2);
    frame1Coords.push_back(detect_ball(fpair.first, nullptr));
    frame2Coords.push_back(detect_ball(fpair.second, nullptr));
    std::cout<<"Bring the ball to (0, 0, 1) and press the space bar"<<std::endl;
    fpair = capture_frames(cam1, cam2);
    frame1Coords.push_back(detect_ball(fpair.first, nullptr));
    frame2Coords.push_back(detect_ball(fpair.second, nullptr));
    std::cout<<"Bring the ball to (0, 1, 0) and press the space bar"<<std::endl;
    fpair = capture_frames(cam1, cam2);
    frame1Coords.push_back(detect_ball(fpair.first, nullptr));
    frame2Coords.push_back(detect_ball(fpair.second, nullptr));
    std::cout<<"Bring the ball to (1, 0, 0) and press the space bar"<<std::endl;
    fpair = capture_frames(cam1, cam2);
    frame1Coords.push_back(detect_ball(fpair.first, nullptr));
    frame2Coords.push_back(detect_ball(fpair.second, nullptr));

    // Obtain rvec and tvec
    solvePnP(worldCoords, frame1Coords, cameraMatrix, distortionCoefficients,rvec1, tvec1);
    solvePnP(worldCoords, frame2Coords, cameraMatrix, distortionCoefficients,rvec2, tvec2);

    // Get rotation matrices
    cv::Mat rotM1(3, 3, CV_64F);
    cv::Mat rotM2(3, 3, CV_64F);
    Rodrigues(rvec1, rotM1);
    Rodrigues(rvec2, rotM2);

    // Find projection matrices
    cv::Mat rotMT1 = rotM1.t();
    cv::Mat rotMT2 = rotM2.t();
    rotMT1.push_back(tvec1.reshape(1, 1));
    rotMT2.push_back(tvec2.reshape(1, 1));
    projMat1 = cameraMatrix * rotMT1.t();
    projMat2 = cameraMatrix * rotMT2.t();
}

void triangulate(Point2f camera1Point, Point2f camera2Point){
    cv::Mat pnt3D(4,1,CV_64F);
    std::vector<Point2d> cam1pnts = {camera1Point};
    std::vector<Point2d> cam2pnts = {camera2Point};
    triangulatePoints(projMat1, projMat2, cam1pnts, cam2pnts, pnt3D);
    float x = pnt3D.at<double>(0,0) / pnt3D.at<double>(3,0);
    float y = pnt3D.at<double>(1,0) / pnt3D.at<double>(3,0);
    float z = pnt3D.at<double>(2,0) / pnt3D.at<double>(3,0);

    std::cout << "Predicted location is: ("<<x<<", "<<y<<", "<<z<<")"<<std::endl;
}

int main(){
    configure_cameras();
    calibrate(capArr[idxToCam[0]], capArr[idxToCam[1]]);
    Mat frame1, frame2;
    for(;;){
        capArr[idxToCam[0]].read(frame1);
        capArr[idxToCam[1]].read(frame2);
        bool success = false;
        Point2f p1 = detect_ball(frame1, &success);
        if(success){
            Point2f p2 = detect_ball(frame2, &success);
            if(success){
                triangulate(p1, p2);
            }
        }
    }
}