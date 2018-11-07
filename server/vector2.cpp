#include "vector2.h"

Vector2::Vector2(double a, double b){
  x = a;
  y = b;
}

Vector2 operator+(const Vector2& a, const Vector2& b){
  return Vector2(a.x + b.x, a.y + b.y);
}

Vector2 operator-(const Vector2& a, const Vector2& b){
  return Vector2(a.x - b.x, a.y - b.y);
}