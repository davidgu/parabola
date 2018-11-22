#include <iostream>
#include <fstream>

#include "opencv2/calib3d.hpp"
#include "opencv2/opencv.hpp"

using namespace cv;

const bool DAYTIME = false;

const Mat cameraMatrix = (Mat1d(3, 3)<< 4.4740797894345377e+02, 0.0, 320.0, 0.0, 4.4740797894345377e+02, 213.0, 0.0, 0.0, 1.0);
const Mat distortionCoefficients = (Mat1d(1, 5)<< 1.7657493713843387e-02, -1.6646113914955049e-01, 0.0, 0.0, 2.6525645359464572e-01); 

Mat projMat1, projMat2;
Point3f virtualOrigin;
float scale;

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

Point2f detect_ball(Mat frame, bool *success){
  GaussianBlur(frame, frame, Size(5, 5), 4, 4);
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

        int key = waitKey(1);

        // space bar
        if(key == 32){
            return std::pair<Mat, Mat>(frame1, frame2);
        }

        imshow("Camera 1", frame1);
        imshow("Camera 2", frame2);
    }
}

Point3f triangulate(Point2f camera1Point, Point2f camera2Point){
    cv::Mat pnt3D(4,1,CV_64F);
    std::vector<Point2d> cam1pnts = {camera1Point};
    std::vector<Point2d> cam2pnts = {camera2Point};
    triangulatePoints(projMat1, projMat2, cam1pnts, cam2pnts, pnt3D);
    float x = pnt3D.at<double>(0,0) / pnt3D.at<double>(3,0);
    float y = pnt3D.at<double>(1,0) / pnt3D.at<double>(3,0);
    float z = pnt3D.at<double>(2,0) / pnt3D.at<double>(3,0);

    return Point3f(x,y,z);
}

// Gets the projection matrices for the two cameras
void calibrate(VideoCapture cam1, VideoCapture cam2){
    Mat frames1[4];
    Mat frames2[4];
    
    Point2f cam1Origin;
    Point2f cam2Origin;

    Point2f cam1XPlus1;
    Point2f cam2XPlus1;

    Mat rvec1, rvec2, tvec1, tvec2;

    std::vector<Point3f> worldCoords = {
        Point3f(0,0,0),
        Point3f(0,0,1),
        Point3f(0,-1,0),
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
    cam1Origin = (detect_ball(fpair.first, nullptr));
    cam2Origin = (detect_ball(fpair.second, nullptr));
    std::cout<<"Bring the ball to (0, 0, 1) and press the space bar"<<std::endl;
    fpair = capture_frames(cam1, cam2);
    frame1Coords.push_back(detect_ball(fpair.first, nullptr));
    frame2Coords.push_back(detect_ball(fpair.second, nullptr));
    std::cout<<"Bring the ball to (0, -1, 0) and press the space bar"<<std::endl;
    fpair = capture_frames(cam1, cam2);
    frame1Coords.push_back(detect_ball(fpair.first, nullptr));
    frame2Coords.push_back(detect_ball(fpair.second, nullptr));
    std::cout<<"Bring the ball to (1, 0, 0) and press the space bar"<<std::endl;
    fpair = capture_frames(cam1, cam2);
    frame1Coords.push_back(detect_ball(fpair.first, nullptr));
    frame2Coords.push_back(detect_ball(fpair.second, nullptr));
    cam1XPlus1 = (detect_ball(fpair.first, nullptr));
    cam2XPlus1 = (detect_ball(fpair.second, nullptr));

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

void calibrate_vorigin(VideoCapture cam1, VideoCapture cam2){
    std::vector<Point2f> frame1Coords;
    std::vector<Point2f> frame2Coords;

    Point2f cam1Origin;
    Point2f cam2Origin;

    Point2f cam1XPlus1;
    Point2f cam2XPlus1;

    std::pair<Mat, Mat> fpair;
    std::cout<<"Coordinates will be expressed as (X, Y, Z)"<<std::endl;
    std::cout<<"Make sure the ball is visible by both cameras at all times."<<std::endl;
    std::cout<<"Bring the ball to (0, 0, 0) and press the space bar"<<std::endl;
    fpair = capture_frames(cam1, cam2);
    cam1Origin = (detect_ball(fpair.first, nullptr));
    cam2Origin = (detect_ball(fpair.second, nullptr));

    std::cout<<"Bring the ball to (1, 0, 0) and press the space bar"<<std::endl;
    fpair = capture_frames(cam1, cam2);
    cam1XPlus1 = (detect_ball(fpair.first, nullptr));
    cam2XPlus1 = (detect_ball(fpair.second, nullptr));

    // Find virtual origin
    virtualOrigin = triangulate(cam1Origin, cam2Origin);

    // Find scaling factor
    Point3f virtualXPlus1 = triangulate(cam1XPlus1, cam2XPlus1) - virtualOrigin;
    if(scale !=0){
        scale = 1/(virtualXPlus1.x);
    }
    else{
        scale = 1;
    }
}

bool fileExists(const std::string name)
{
    std::ifstream infile(name.c_str());
    return infile.good();
}

// Checks if a camera configuration exists, and loads it if it does
int load_configuration(const std::string camProjMat1, 
                        const std::string camProjMat2,
                        const std::string vOrigin){
    int retStatus = 0;
    if(fileExists(camProjMat1) && fileExists(camProjMat2)){
        FileStorage cpm1fs(camProjMat1, FileStorage::READ);
        cpm1fs["data"] >> projMat1;

        FileStorage cpm2fs(camProjMat2, FileStorage::READ);
        cpm2fs["data"] >> projMat2;

        retStatus++;
        if(fileExists(vOrigin)){
            FileStorage vo(vOrigin, FileStorage::READ);
            vo["data"] >> virtualOrigin;
        }
    }

    // retStatus will be 2 if camera matrixes and virtual origin data both exist
    // retStatus will be 1 if only camera matrixes exist
    // retStatus will be 0 if no configuration data exists
    return retStatus;
}

// Saves camera projection matrices and virtual origin data
// Does not overwrite
void save_configuration(const std::string camProjMat1,
                        const std::string camProjMat2,
                        const std::string vOrigin){
    if(!fileExists(camProjMat1)){
        FileStorage fs(camProjMat1, FileStorage::WRITE);
        fs << "data" << projMat1;
    }
    if(!fileExists(camProjMat2)){
        FileStorage fs(camProjMat2, FileStorage::WRITE);
        fs << "data" << projMat2;
    }
    if(!fileExists(vOrigin)){
        FileStorage fs(vOrigin, FileStorage::WRITE);
        fs << "data" << vOrigin;
    }
}

int main(){
    configure_cameras();
    // No saved data exists
    if(load_configuration("cpm1.xml", "cpm2.xml", "vo.xml")==0){
        calibrate(capArr[idxToCam[0]], capArr[idxToCam[1]]);
        calibrate_vorigin(capArr[idxToCam[0]], capArr[idxToCam[1]]))
        save_configuration();
    }
    // Virtual origin data does not exist 
    else if(load_configuration("cpm1.xml", "cpm2.xml", "vo.xml") == 1){
        calibrate_vorigin(capArr[idxToCam[0]], capArr[idxToCam[1]]))
        save_configuration();
    }
    // All data exists
    else if(load_configuration("cpm1.xml", "cpm2.xml", "vo.xml") == 2){
        // Do nothing
    }
    else{
        // Fail if an invalid load configuration value is returned
        assert(false);
    }

    Mat frame1, frame2;
    for(;;){
        capArr[idxToCam[0]].read(frame1);
        capArr[idxToCam[1]].read(frame2);
        bool success = false;
        Point2f p1 = detect_ball(frame1, &success);
        if(success){
            Point2f p2 = detect_ball(frame2, &success);
            if(success){
                Point3f predict = scale*(triangulate(p1, p2)-virtualOrigin);
                std::cout << "Virtual origin: "<<virtualOrigin;
                std::cout << "Predicted location is: ("<<predict.x<<", "<<predict.y<<", "<<predict.z<<")"<<std::endl;
            }
        }
    }
}
