#ifndef VECTOR3_H
#define VECTOR3_H

class Vector2{
  private:
    double x, y;
  public:
    Vector2(double a, double b);
    friend Vector2 operator+(const Vector2& a, const Vector2& b);
    friend Vector2 operator-(const Vector2& a, const Vector2& b);
};

Vector2 operator+(const Vector2& a, const Vector2& b);
Vector2 operator-(const Vector2& a, const Vector2& b);

#endif
