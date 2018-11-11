#ifndef VECTOR3_H
#define VECTOR3_H

#include <iostream>
#include <stdexcept>
#include <cmath>

class Vector3{
  public:
    double x, y, z;
    Vector3(double a, double b, double c);

    static double dot(const Vector3 a, const Vector3 b);
    static Vector3 proj(const Vector3 a, const Vector3 b);

    friend Vector3 operator+(const Vector3& a, const Vector3& b);
    friend Vector3 operator-(const Vector3& a, const Vector3& b);
    friend Vector3 operator/(const Vector3& a, double b);
    friend Vector3 operator*(const Vector3& a, double b);
    friend bool operator==(const Vector3& a, const Vector3& b);
    friend std::ostream& operator<<(std::ostream& out, const Vector3& a);
};
Vector3 operator+(const Vector3& a, const Vector3& b);
Vector3 operator-(const Vector3& a, const Vector3& b);
Vector3 operator/(const Vector3& a, double b);
Vector3 operator*(const Vector3& a, double b);
bool operator==(const Vector3& a, const Vector3& b);
std::ostream& operator<<(std::ostream& out, const Vector3& a);


#endif
