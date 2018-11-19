#include "../src/vector2.hpp"
#include <assert.h>
#include <iostream>

int main(){
 Vector2 arr[] = {Vector2(-10.0,20.0),Vector2(-20.0,-10.0),Vector2(10.0,-20.0),Vector2(20.0,10.0)};
  std::vector<Vector2> coneLocations(arr, arr + sizeof(arr) / sizeof(arr[0]) );
  Vector2 ballLocation = Vector2(10.0,10.0);
  Vector2 newCoords = rotate_ball_loc(coneLocations,ballLocation);
  std::cout<<"newCoords"<<std::endl;
//   assert(newCords == Vector2(
}
