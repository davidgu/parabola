#include "vector3.h"

Vector3::Vector3(double a, double b, double c){
  x = a;
  y = b;
  z = c;
}

Vector3 operator+(const Vector3& a, const Vector3& b){
  return Vector3(a.x + b.x, a.y + b.y, a.z + a.z);
}

Vector3 operator-(const Vector3& a, const Vector3& b){
  return Vector3(a.x - b.x, a.y - b.y, a.z - a.z);
}