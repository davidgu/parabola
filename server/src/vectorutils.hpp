#ifndef VECTORUTILS_H
#define VECTORUTILS_H

#include <utility>
#include <exception>

#include "cameraconfig.hpp"
#include "vector3.hpp"
#include "vector2.hpp"

Vector2 pnormalize_origin(Vector2 vec);

std::pair <Vector3,Vector3> build_vector(CameraConfig cam, Vector3 cam_wpos, 
                                        Vector2 ball_ppos);

Vector3 middle_of_perp(const Vector3 v1, const Vector3 d1, const Vector3 d2);

Vector3 find_intersection(const Vector3 v1, const Vector3 v2, const Vector3 v3,
                        const Vector3 d1, const Vector3 d2,const Vector3 d3);

#endif