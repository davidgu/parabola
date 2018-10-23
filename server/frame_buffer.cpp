#include <queue>
#include <exception>

struct Vector3{
  double x, y, z;
  Vector3(double a, double b, double c){
    x = a;
    y = b;
    z = c;
  }
};

class FrameBuffer{
  private:
    std::queue<double> x_loc;
    std::queue<double> y_loc;
    std::queue<double> z_loc;

  public:
    void update_pos(double val, int dimension){ // 0 = x, 1 = y, 2 = z
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
    bool buffer_empty(){
      return x_loc.empty() || y_loc.empty() || z_loc.empty();
    }

    Vector3 get_pos(){
      if(buffer_empty()){
        int x = x_loc.front();
        int y = y_loc.front();
        int z = z_loc.front();
        x_loc.pop();
        y_loc.pop();
        z_loc.pop();
        return Vector3(x, y, z);
      }
      else{
        throw std::runtime_error("Cannot get pos when buffer is empty!");
      }
    }
};