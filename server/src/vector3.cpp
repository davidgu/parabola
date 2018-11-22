#include "vector3.hpp"

Vector3::Vector3(double a, double b, double c){
  x = a;
  y = b;
  z = c;
}

Vector3::Vector3(){
  x = 0.0;
  y = 0.0;
  z = 0.0;
}

std::string Vector3::to_string(){
  std::string ret = "<"+std::to_string(x)+","+std::to_string(y)+","+std::to_string(z)+">";
  return ret;
}

std::string Vector3::to_json(){
  std::string x_str = std::to_string(x);
  std::string y_str = std::to_string(y);
  std::string z_str = std::to_string(z);

  std::string ret = "{\"x\":"+x_str+", \"y\":"+y_str+", \"z\":"+z_str+"}";
  return ret;
}

Vector3 operator+(const Vector3& a, const Vector3& b){
  return Vector3(a.x + b.x, a.y + b.y, a.z + b.z);
}
Vector3 operator-(const Vector3& a, const Vector3& b){
  return Vector3(a.x - b.x, a.y - b.y, a.z - b.z);
}
Vector3 operator/(const Vector3& a, double b){
  return Vector3(a.x/b, a.y/b, a.z/b);
}
Vector3 operator*(const Vector3& a, double b){
  return Vector3(a.x*b, a.y*b, a.z*b);
}
bool operator==(const Vector3& a, const Vector3& b){
  return (a.x==b.x)&&(a.y==b.y)&&(a.z==b.z);
}
std::ostream& operator<<(std::ostream& out, const Vector3& a){
  out << "(" << a.x << " " << a.y << " " << a.z << ")";
  return out;
}

double Vector3::dot(const Vector3 a, const Vector3 b){
  return (a.x)*(b.x) + (a.y)*(b.y) + (a.z)*(b.z);
}

Vector3 Vector3::cross(const Vector3 a, const Vector3 b){
  return Vector3(
    (a.y * b.z) - (a.z * b.y),
    (a.z * b.x) - (a.x * b.z),
    (a.x * b.y) - (a.y * b.x)
  );
}

double Vector3::magnitude(const Vector3 a){
  return sqrt((a.x * a.x) + (a.y * a.y) + (a.z * a.z));
}

Vector3 Vector3::point3f_to_vector3(cv::Point3f in){
  return Vector3(
    (double)in.x,
    (double)in.y,
    (double)in.z
  );
}

Vector3 Vector3::proj(const Vector3 a, const Vector3 b){
  // projects a onto b
  double b_sum_abs_comps = (b.x*b.x) + (b.y*b.y) + (b.z*b.z);
  double scalar = dot(a,b) / b_sum_abs_comps;
  return b*scalar;
}

Vector3 Vector3::perp(const Vector3 a, const Vector3 b){
  // perp from a to b
  return b - Vector3::proj(a,b);
}
