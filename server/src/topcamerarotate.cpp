#include <iostream>
#include <assert>
#include <math.h>
#include "opencv2/opencv.hpp"
#include "opencv2/tracking.hpp"
#include "../src/vector2.hpp"

using namespace cv;

Vector2 rotate_ball_loc(std::vector<Vector2> > coneLocations, Vector2 ballLocation){
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
    }
  }


  double slope = (lowestYPair.y - lowestXPair.y)/(lowestYPair.x - lowestXPair.x);
  double yDiff = sLowestYPair.y - lowestYPair.y;
  double theta = asin(yDiff/slope);
  
  Vector2 ans;
  ans.x cos(theta)*ballLocation.x - sin(theta)*ballLocation.y;
  ans.y= sin(theta)*ballLocation.x + cos(theta)*ballLocation.y;
  return ans;
}
