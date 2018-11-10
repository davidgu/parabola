#ifndef VECTOR3_H
#define VECTOR3_H

class Vector3{
  private:
    double x, y, z;
  public:
    Vector3(double a, double b, double c);

    double dot(const Vector3 *a, const Vector3 *b);
    double sum_components_squared(const Vector3 *a);
    Vector3 projection(const Vector3 *a, const Vector3 *b);

    friend Vector3 operator+(const Vector3& a, const Vector3& b);
    friend Vector3 operator-(const Vector3& a, const Vector3& b);
    friend void operator/(const Vector3& a, double b);
};


#endif
