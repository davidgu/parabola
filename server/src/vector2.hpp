#ifndef VECTOR2_H
#define VECTOR2_H

class Vector2{
  public:
    double x, y;
    Vector2(double a, double b);
    Vector2();
    friend Vector2 operator+(const Vector2& a, const Vector2& b);
    friend Vector2 operator-(const Vector2& a, const Vector2& b);
};

Vector2 operator+(const Vector2& a, const Vector2& b);
Vector2 operator-(const Vector2& a, const Vector2& b);

#endif
