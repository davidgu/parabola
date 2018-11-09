#include "cameraconfig.h"

CameraConfig::CameraConfig(int i, Vector3 p){
    index = i;
    position = p;
}

CameraConfig::get_index(){
    return index;
}

CameraConfig::get_position(){
    return position;
}