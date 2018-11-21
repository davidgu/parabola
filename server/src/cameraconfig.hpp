#ifndef CAMERACONFIG_H
#define CAMERACONFIG_H

#include <string>

#include "opencv2/calib3d.hpp"
#include "vector3.hpp"
#include "opencv2/opencv.hpp"

class CameraConfig{
    private:
        int index;
        Vector3 position;
        cv::VideoCapture capArr[4];
        cv::Mat frames[4];

        // idx 0 = top, idx 1 = up, idx 2 = right
        int camIdx[3]; // Each index describes the cam number

        // This will contain more data about the camera, such as dewarp
        // parameters.
    public:
        CameraConfig(int i, Vector3 p);
        CameraConfig();
        int get_index();
        Vector3 get_position();
        void load_from_file(std::string path);
        int countCameras();
        int build_camera_config();
        cv::Mat detect_cones(cv::Mat, int coneCali[2][3]);
        void fix_cam_rot();
        void match_cams();
};

#endif
