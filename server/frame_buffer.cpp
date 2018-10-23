#include <queue>

struct Vector3{
  double x, y, z;
  Vector3(double a, double b, double c){
    x = a;
    y = b;
    z = c;
  }
};
std::queue<double> x_loc;
std::queue<double> y_loc;
std::queue<double> z_loc;

void update_pos(double val, int idx){ // 0 = x, 1 = y, 2 = z
  if(val == 0){
    x_loc.push(val);
  }else if(val == 1){
    y_loc.push(val);
  }else{
    z_loc.push(val);
  }

  if(!x_loc.empty() && !y_loc.empty() && z_loc.empty()){
    int cur_x = x_loc.pop();
    int cur_y = y_loc.pop();
    int cur_z = z_loc.pop();
  }
}

int main(){
}
