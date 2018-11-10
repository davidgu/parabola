#include "Vector3.h"
#include <exception>

Vector3* middle_of_perp(const Vector3<double> v1, const Vector3<double> d1, const Vector3<double> d2){
  //calculate proj
  Vector3 l1 = d2 - d1;
  Vector3 *proj = projection(l1,v1);
  Vector3 *perp = l1 - proj;
  double p1 = d2-perp;
  double p2 = d2;
  Vector3 ans = (p2-p1)/2;
  return;
}

Vector3<double> find_intersection(const Vector3<double> v1, const Vector3<double> v2, const Vector3<double> v3,const Vector3<double> d1,const Vector3<double> d2,const Vector3<double> d3){
  Vector3 *mid1 = middle_of_perp(v1,d1,d2);
  Vector3 *mid2 = middle_of_perp(v2,d2,d3);
  Vector3 *mid3 = middle_of_perp(v3,d3,d1);

  //calculate midpoint
  return Vector3((mid1->x + mid2->x + mid3->x)/3, (mid1->y + mid2->y + mid3->y)/3, (mid1->z + mid2->z + mid3->z)/3);
}
