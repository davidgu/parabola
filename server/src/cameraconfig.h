#ifndef CAMERACONFIG_H
#define CAMERACONFIG_H

#include "opencv2/calib3d.hpp"
#include "vector3.h"

class CameraConfig{
    private:
        const int index;
        const Vector3 position;
        // This will contain more data about the camera, such as dewarp
        // parameters.
    public:
        CameraConfig(int i, Vector3 p);
        int get_index();
        Vector3 get_position();
};

#endif