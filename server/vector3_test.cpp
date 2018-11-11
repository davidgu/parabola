#include "vector3.hpp"
#include <assert.h>
#include <iostream>

int main(){
  Vector3 v1 = Vector3(2.0,3.0,4.0);
  Vector3 v2 = Vector3(2.0,3.0,4.0);

  // Equality
  assert(v1==v2);

  // Arithmertic operators
  assert(v1+v2 == Vector3(4.0,6.0,8.0));
  assert(v1-v2 == Vector3(0.0,0.0,0.0));
  assert(v1*2 == Vector3(4.0,6.0,8.0));
  assert(v1/2 == Vector3(1.0,1.5,2.0));

  // dot()
  assert(Vector3::dot(v1, v2) == 29.0);

  // proj() & perp()
  Vector3 v3 = Vector3(-1.0,4.0,2.0);
  Vector3 v4 = Vector3(1.0,0.0,3.0);
  assert(Vector3::proj(v3, v4) == Vector3(0.5, 0.0, 1.5));
  assert(Vector3::perp(v3, v4) == Vector3(0.5, 0.0, 1.5));
}
