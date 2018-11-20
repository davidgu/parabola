#include "../src/vector2.hpp"
#include "../src/topcamerarotate.hpp"
#include <assert.h>
#include <iostream>
#define LOC_EPSILON 0.0001
#define DIST_EPSILON 0.01

#include "opencv2/opencv.hpp"

using namespace cv;

int main(){
  // If you want to make a case where the cube isn't centered on the origin,
  // make sure when you are asserting the length of the points from the middle
  // of the cones, not the origin

  // case 1
  Vector2 arr1[] = {Vector2(-10.0,20.0),Vector2(-20.0,-10.0),Vector2(10.0,-20.0),Vector2(20.0,10.0)};
  std::vector<Vector2> coneLocations1(arr1, arr1 + sizeof(arr1) / sizeof(arr1[0]) );
  Vector2 ballLocation1 = Vector2(10.0,10.0);
  Vector2 newCoords1 = rotate_ball_loc(coneLocations1,ballLocation1);
  assert(fabs(newCoords1.x - 6.32455532) < LOC_EPSILON && fabs(newCoords1.y - 12.64911064) < LOC_EPSILON);
  assert(fabs(sqrt(newCoords1.x*newCoords1.x + newCoords1.y*newCoords1.y) - 14.14213562) < DIST_EPSILON);

  // case 2
  Vector2 arr2[] = {Vector2(10.0,20.0),Vector2(-20.0,10.0),Vector2(-10.0,-20.0),Vector2(20.0,-10.0)};
  std::vector<Vector2> coneLocations2(arr2, arr2 + sizeof(arr2) / sizeof(arr2[0]) );
  Vector2 ballLocation2 = Vector2(-10.0,10.0);
  Vector2 newCoords2 = rotate_ball_loc(coneLocations2,ballLocation2);
  std::cout<<newCoords2.x<<" "<<newCoords2.y<<std::endl;
  assert(fabs(newCoords2.x + 6.32455532) < LOC_EPSILON && fabs(newCoords2.y - 12.64911064) < LOC_EPSILON);
  assert(fabs(sqrt(newCoords2.x*newCoords2.x + newCoords2.y*newCoords2.y) - 14.14213562) < DIST_EPSILON);


  std::cout<<"rotation_dewarping_tests_passed"<<std::endl;
}
