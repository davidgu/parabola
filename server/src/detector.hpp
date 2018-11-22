#include <iostream>
#include <fstream>
#include <limits>

#include "opencv2/calib3d.hpp"
#include "opencv2/opencv.hpp"

using namespace cv;

class Detector{
    private:
        bool DAYTIME;
        //(Mat1d(3, 3)<< 4.4740797894345377e+02, 0.0, 320.0, 0.0, 4.4740797894345377e+02, 213.0, 0.0, 0.0, 1.0);
        Mat cameraMatrix; 
        //(Mat1d(1, 5)<< 1.7657493713843387e-01, -1.6646113914955049e-01, 0.0, 0.0, 2.6525645359464572e-01); 
        Mat distortionCoefficients;

        Mat projMat1, projMat2;
        Point3f virtualOrigin;
        float scale;

        VideoCapture capArr[4];
        int idxToCam[4];

        // Functions
        Mat detect_cones(Mat frame);
        Mat findBiggestBlob(Mat & matImage);
        Mat detect_cones_day(Mat frame);
        Point2f detect_ball(Mat frame, bool *success);
        std::pair<Mat,Mat> capture_frames(VideoCapture cam1, VideoCapture cam2);
        bool fileExists(const std::string name);
        void write_point_data(const std::string fileName, std::vector<Point2f> points);
        Point3f triangulate(Point2f camera1Point, Point2f camera2Point);
    public:
        Detector(bool daytime, Mat cameraMatrix, Mat distortionCoefficients);
        void configure_cameras();
        void calibrate(int camidx1, int camidx2);
        void calibrate_vorigin(int camidx1, int camidx2);
        int load_configuration(const std::string camProjMat1, 
                                const std::string camProjMat2,
                                const std::string vOrigin);
        void save_configuration(const std::string camProjMat1,
                                const std::string camProjMat2,
                                const std::string vOrigin);
        // Returns (FLOAT_MAX, FLOAT_MAX, FLOAT_MAX) on failure
        Point3f get_normalized_position();
};