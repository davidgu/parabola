#include "vectorutils.hpp"
#include <exception>

const int IMG_WIDTH = 1920;
const int IMG_HEIGHT = 1080;

// Converts a pixel coordinate from lower left corner origin
// to center origin
Vector2 pnormalize_origin(Vector2 vec){
    Vector2 normalized = Vector2(vec.x-IMG_WIDTH, vec.y-IMG_HEIGHT);
    return normalized;
}

std::pair <Vector3,Vector3> build_vector(CameraConfig cam, Vector3 cam_wpos, 
                                        Vector2 ball_ppos){
    // Returns the line vector representing the possible positions of the
    // detected ball in the format (point, direction)
    // wpos -> World Position
    // ppos -> Pixel Position
    // ball_ppos Vector2 is given in terms of normalized pixel coordinates
    Vector3 direction;
    Vector3 point;
    switch(cam.get_index()){
        case 0:
            direction = Vector3(ball_ppos.x, ball_ppos.y, 0);
            point = cam.get_position;
            break;
        case 1:
            direction = Vector3(0, ball_ppos.x, ball_ppos.y);
            point = cam.get_position;
            break;
        case 2:
            direction = Vector3(ball_ppos.x, 0, ball_ppos.y);
            point = cam.get_position;
            break;
        default:
            throw std::invalid_argument("Invalid camera. This should never happen.");
    }
    std::pair <Vector3, Vector3> ret (point, direction);
    return ret;
}
