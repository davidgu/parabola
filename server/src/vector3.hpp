#ifndef VECTOR3_H
#define VECTOR3_H

#include <iostream>
#include <stdexcept>
#include <cmath>
#include <string>

#include "opencv2/opencv.hpp"

class Vector3{
  public:
    double x, y, z;
    Vector3(double a, double b, double c);
    Vector3();
    std::string to_string();
    std::string to_json();

    static double dot(const Vector3 a, const Vector3 b);
    static Vector3 cross(const Vector3 a, const Vector3 b);
    static Vector3 proj(const Vector3 a, const Vector3 b);
    static Vector3 perp(const Vector3 a, const Vector3 b);
    static double magnitude(const Vector3 a);
    static Vector3 point3f_to_vector3(cv::Point3f in);

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
