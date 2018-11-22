#ifndef CAMERACONFIG_H
#define CAMERACONFIG_H

#include <string>

#include "opencv2/calib3d.hpp"
#include "vector3.hpp"
#include "vector2.hpp"
#include "opencv2/opencv.hpp"

class CameraConfig{
    private:

        const bool MANUALLY_MODIFY_MASKS = false;       
        const int NUM_CAMS = 2;       

        int index;
        Vector3 position;
        cv::VideoCapture capArr[4];
       
        // Measurements are in metric units 
        double topCameraHeight = 1.5;
        double northCameraX = 2.0;
        double northCameraY = 2.0;
        double eastCameraZ = 2.0;
        double eastCameraY = 2.0;

        // idx 0 = Top, idx 1 = North, idx 2 = East
        int camIdx[3]; // Each index describes the cam number
        cv::Mat frames[4];
        int rotCamIdx[3];
        int coneCali[2][3] = {{1,150,200},{15,255,255}};
        int ballCali1[2][3] = {{0,100,100},{10,255,255}};
        int ballCali2[2][3] = {{105,80,80},{180,255,255}};
        double topRotTheta;        

    public:
        static void *currentlyEditingConfig;
        std::vector<Vector2> coneLocations;

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
        void calibrate_cones();
        void calibrate_ball(int maskId); // id1: first mask, id2: second mask
        //void detect_cones_helper(int event, int x, int y, int flags, void* param);
        void detect_cones();
        static void set_currently_editing_cfg(CameraConfig *cfgptr);
        void calc_top_rot_theta();

};

#endif
