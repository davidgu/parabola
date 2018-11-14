#include "cameraconfig.hpp"

CameraConfig::CameraConfig(int i, Vector3 p){
    index = i;
    position = p;
}

int CameraConfig::get_index(){
    return index;
}

Vector3 CameraConfig::get_position(){
    return position;
}

void CameraConfig::load_from_file(std::string path){
    // Unimplemented
}