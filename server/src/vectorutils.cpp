#include "vectorutils.hpp"

const int IMG_WIDTH = 1920;
const int IMG_HEIGHT = 1080;

// Converts a pixel coordinate from lower left corner origin
// to center origin
Vector2 pnormalize_origin(Vector2 vec){
    Vector2 normalized = Vector2(vec.x-IMG_WIDTH, vec.y-IMG_HEIGHT);
    return normalized;
}

std::pair <Vector3,Vector3> build_vector(int camIdx, Vector3 cam_wpos, 
                                        Vector2 ball_ppos){
    // Returns the line vector representing the possible positions of the
    // detected ball in the format (point, direction)
    // wpos -> World Position
    // ppos -> Pixel Position
    // ball_ppos Vector2 is given in terms of normalized pixel coordinates
    Vector3 direction;
    Vector3 point;

    switch(camIdx){
        // Top camera
        case 0:
            direction = Vector3(ball_ppos.x, 0, ball_ppos.y);
            point = cam_wpos;
            break;
        // North camera
        case 1:
            direction = Vector3(0, ball_ppos.y, ball_ppos.x);
            point = cam_wpos;
            break;
        // East camera
        case 2:
            direction = Vector3(ball_ppos.x, ball_ppos.y, 0);
            point = cam_wpos;
            break;
        default:
            throw std::invalid_argument("Invalid camera. This should never happen.");
    }
    std::pair <Vector3, Vector3> ret (point, direction);
    return ret;
}

Vector3 closest_approach(const std::pair<Vector3, Vector3> l1,
                         const std::pair<Vector3, Vector3> l2){
    Vector3 n_dir = Vector3::cross(l1.second, l2.second);
    Vector3 n_hat = n_dir/Vector3::magnitude(n_dir);
    double n_mag = fabs(Vector3::dot((l1.first - l2.first), n_hat));
    Vector3 point_of_closest_approach = n_dir * n_mag;
    return point_of_closest_approach;
}

Vector3 find_intersection(const std::pair<Vector3, Vector3> l1,
                            const std::pair<Vector3, Vector3> l2){
  return closest_approach(l1, l2);
}

Vector3 find_intersection(const std::pair<Vector3, Vector3> l1,
                            const std::pair<Vector3, Vector3> l2,
                            const std::pair<Vector3, Vector3> l3){
  
  Vector3 mid1 = closest_approach(l1, l2);
  Vector3 mid2 = closest_approach(l2, l3);
  Vector3 mid3 = closest_approach(l1, l3);

  return Vector3(
        (mid1.x + mid2.x + mid3.x)/3, 
        (mid1.y + mid2.y + mid3.y)/3, 
        (mid1.z + mid2.z + mid3.z)/3
      );
}