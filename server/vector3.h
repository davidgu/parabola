#ifndef VECTOR3_H
#define VECTOR3_H
//we should rename this to vector3.hpp
class Vector3{
  private:
    double x, y, z;
  public:
    Vector3(double a, double b, double c);

    double dot(const Vector3 *a, const Vector3 *b);
    double sum_components_squared(const Vector3 *a);
    Vector3* projection(const Vector3 *a, const Vector3 *b);

    friend Vector3 operator+(const Vector3& a, const Vector3& b);
    friend Vector3 operator-(const Vector3& a, const Vector3& b);
    friend void operator/(Vector3& a, double b);
    friend void operator*(Vector3& a, double b);
};
Vector3 operator+(const Vector3& a, const Vector3& b);
Vector3 operator-(const Vector3& a, const Vector3& b);
void operator/(Vector3& a, double b);
void operator*(Vector3& a, double b);


#endif
