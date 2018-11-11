#include "vector3.h"
#include <stdexcept>
Vector3::Vector3(double a, double b, double c){
  x = a;
  y = b;
  z = c;
}
Vector3 Vector3::operator+(const Vector3& a, const Vector3& b){
  return Vector3(a.x + b.x, a.y + b.y, a.z + a.z);
}
Vector3 Vector3::operator-(const Vector3& a, const Vector3& b){
  return Vector3(a.x - b.x, a.y - b.y, a.z - a.z);
}
Vector3 Vector3::operator/(Vector3 *a, double b){
  return Vector3(a.x/b, a.y/b, a.z/b)
}
Vector3 Vector3::operator*(Vector3 *a, double b){
  return Vector3(a.x*b, a.y*b, a.z*b)
}

double Vector3::dot(const Vector3 *a, const Vector3 *b){
  return (a->x)*(b->x) + (a->y)*(b->y) + (a->z)*(b->z);
}

double Vector3::sum_components_squared(const Vector3 *a){
  return (a->x)*(a->x) + (a->y)*(a->y) + (a->z)*(a->z);
}


Vector3* Vector3::projection(const Vector3 *a, const Vector3 *b){
  // I need to watch out for the case where the projection has no width
  if(a->x == b->x && a->x == b->x&&a->x == b->x) throw std::runtime_error("projection(): can't project same vectors");

  // project a onto b
  double scalar = dot(a,b) / sum_components_squared(b);
  Vector3 *newVec = new Vector3(b->x,b->y,b->z);
  *newVec = (*newVec) * scalar;
  return newVec;
}


