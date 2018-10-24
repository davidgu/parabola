#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "vector3.h"

class FrameBuffer{
  private:
    std::queue<double> x_loc;
    std::queue<double> y_loc;
    std::queue<double> z_loc;

  public:
    void update_pos(double val, int dimension); 
    bool buffer_empty();
    Vector3 get_pos();
};

#endif