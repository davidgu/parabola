#include <queue>
#include <exception>

#include "vector3.hpp"
#include "framebuffer.hpp"

void FrameBuffer::update_pos(double val, int dimension){ // 0 = x, 1 = y, 2 = z
  if(dimension == 0){
    x_loc.push(val);
  }
  else if(dimension == 1){
    y_loc.push(val);
  }
  else if(dimension == 2){
    z_loc.push(val);
  }
  // Something has gone badly wrong
  else{
    throw std::invalid_argument("Dimension must be 0, 1, or 2!");
  }
}

// Call this method before attempting to get_pos()
bool FrameBuffer::buffer_empty(){
  return x_loc.empty() || y_loc.empty() || z_loc.empty();
}

Vector3 FrameBuffer::get_pos(){
  if(!buffer_empty()){
    double x = x_loc.front();
    double y = y_loc.front();
    double z = z_loc.front();
    x_loc.pop();
    y_loc.pop();
    z_loc.pop();
    return Vector3(x, y, z);
  }
  else{
    throw std::runtime_error("Cannot get pos when buffer is empty!");
  }
}
