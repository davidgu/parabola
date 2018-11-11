#include "vector3.h"

Vector3::Vector3(double a, double b, double c){
  x = a;
  y = b;
  z = c;
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

Vector3 Vector3::proj(const Vector3 a, const Vector3 b){
  double b_sum_abs_comps = (b.x*b.x) + (b.y*b.y) + (b.z*b.z);
  double scalar = dot(a,b) / b_sum_abs_comps;
  return b*scalar;
}

Vector3 Vector3::perp(const Vector3 a, const Vector3 b){
  return b - Vector3::proj(a,b);
}
