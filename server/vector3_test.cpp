#include "vector3.h"
#include <assert.h>

int main(){
  Vector3 v1 = Vector3(2.0,3.0,4.0);
  Vector3 v2 = Vector3(2.0,3.0,4.0);
  //arithmertic operators
  assert(v1+v2 == Vector3(4.0,6.0,8.0));
  assert(v1-v2 == Vector3(0.0,0.0,0.0));
  assert(v1*v2 == Vector3(4.0,9.0,16.0));
  assert(v1/v2 == Vector3(1.0,1.0,1.0));

  //dot()
  assert(dot() == 28.0);

  //sum_components_squared()
  assert(sum_components_squared(v1) == 4.0 + 9.0 + 16.0);

  //projection()

}
