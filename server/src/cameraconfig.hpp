#ifndef CAMERACONFIG_H
#define CAMERACONFIG_H

#include <string>

#include "opencv2/calib3d.hpp"
#include "vector3.hpp"

class CameraConfig{
    private:
        int index;
        Vector3 position;
        // This will contain more data about the camera, such as dewarp
        // parameters.
    public:
        CameraConfig(int i, Vector3 p);
        int get_index();
        Vector3 get_position();
        void load_from_file(std::string path);
};

#endif