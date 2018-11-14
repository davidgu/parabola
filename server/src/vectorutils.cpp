#include "vectorutils.hpp"

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
            point = cam.get_position();
            break;
        case 1:
            direction = Vector3(0, ball_ppos.x, ball_ppos.y);
            point = cam.get_position();
            break;
        case 2:
            direction = Vector3(ball_ppos.x, 0, ball_ppos.y);
            point = cam.get_position();
            break;
        default:
            throw std::invalid_argument("Invalid camera. This should never happen.");
    }
    std::pair <Vector3, Vector3> ret (point, direction);
    return ret;
}

Vector3 middle_of_perp(const Vector3 v1, const Vector3 d1, const Vector3 d2){
  Vector3 l1 = d2 - d1;
  Vector3 proj = Vector3::proj(l1,v1);
  Vector3 perp = l1 - proj;
  Vector3 p1 = d2-perp;
  Vector3 p2 = d2;
  // Does this not simplify to (d2) - (d2-perp) == perp?
  Vector3 ans = (p2-p1)/2;
  return ans;
}

Vector3 find_intersection(const Vector3 v1, const Vector3 v2, const Vector3 v3,
                        const Vector3 d1, const Vector3 d2,const Vector3 d3){
  Vector3 mid1 = middle_of_perp(v1,d1,d2);
  Vector3 mid2 = middle_of_perp(v2,d2,d3);
  Vector3 mid3 = middle_of_perp(v3,d3,d1);

  return Vector3(
        (mid1.x + mid2.x + mid3.x)/3, 
        (mid1.y + mid2.y + mid3.y)/3, 
        (mid1.z + mid2.z + mid3.z)/3
      );
}