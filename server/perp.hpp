#include "Vector3.h"

double dot(const Vector3 &a, const Vector3 &b);
double half_euclidean_dist(const Vector3 &a);
Vector3* projection(const Vector3 &a, const Vector3 &b);
pair<double,double> middle_of_perp(const Vector3<double> v1, const Vector3<double> v2, const Vector3<double> d1, const Vector3<double> d2);
Vector3<double> find_intersection(const Vector3<double> v1, const Vector3<double> v2, const Vector3<double> v3,cons    t Vector3<double> d1,const Vector3<double> d2,const Vector3<double> d3);
