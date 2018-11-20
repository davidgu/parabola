#include <iostream>
#include <assert.h>
#include <math.h>
#include "opencv2/opencv.hpp"
#include "opencv2/tracking.hpp"
#include "vector2.hpp"

using namespace cv;

Vector2 rotate_ball_loc(std::vector<Vector2> coneLocations, Vector2 ballLocation){
  // given the locations of the cones, returns the ball's catesian location
  // Note: The camera must be within 90 degrees of facing in the right orientation
  assert(coneLocations.size() == 4);

  // calculate the midpoint of the cones, needed to find the origin
  Vector2 midpoint;
  for(int i = 0 ; (unsigned) i < coneLocations.size();i++){
    midpoint.x += coneLocations[i].x;
    midpoint.y += coneLocations[i].y;
  }
  midpoint.x/=4;
  midpoint.y/=4;


  // calculate which direction to rotate and how much
  double lowestY = 999999.0;
  double sLowestY = 999999.0;
  Vector2 lowestYPair;
  Vector2 sLowestYPair;

  // finds the lowest point
  for(int i = 0; (unsigned) i < coneLocations.size();i++){
    double x = coneLocations[i].x;
    double y = coneLocations[i].y;
    if( y < lowestY){
      // update the second lowest 
      sLowestY = lowestY;
      sLowestYPair = lowestYPair;
      // update the lowest pair
      lowestY = y;
      lowestYPair = coneLocations[i];
    }else if(y < sLowestY){
      sLowestY = y;
      sLowestYPair = coneLocations[i];
    }
  }

  double xDiff = -1;
  double yDiff = -1;
  // std::cout<<lowestYPair.x<<" "<<lowestYPair.y<<std::endl;
  // std::cout<<sLowestYPair.x<<" "<<sLowestYPair.y<<std::endl;
  if(lowestYPair.x > sLowestYPair.x){ // The lower point it to the bottom right
    xDiff = lowestYPair.x - sLowestYPair.x;
    yDiff = sLowestYPair.y - lowestYPair.y;
  }else{
    xDiff = sLowestYPair.x - lowestYPair.x;
    yDiff = sLowestYPair.y - lowestYPair.y;
  }


  double theta = atan(yDiff/xDiff);
  if(lowestYPair.x < sLowestYPair.x) theta=theta * -1.0;
  // std::cout<<yDiff/xDiff<<std::endl;
  Vector2 ans;

  // here we are multiplying the location vector by a rotation matrix
  ans.x = cos(theta)*ballLocation.x - sin(theta)*ballLocation.y;
  ans.y = sin(theta)*ballLocation.x + cos(theta)*ballLocation.y;
  return ans;
}
