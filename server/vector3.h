#ifndef VECTOR3_H
#define VECTOR3_H

class Vector3{
  private:
    double x, y, z;
  public:
    Vector3(double a, double b, double c);
    friend Vector3 operator+(const Vector3& a, const Vector3& b);
    friend Vector3 operator-(const Vector3& a, const Vector3& b);
};

Vector3 operator+(const Vector3& a, const Vector3& b);
Vector3 operator-(const Vector3& a, const Vector3& b);
#endif
