#ifndef VECTORUTILS_H
#define VECTORUTILS_H

#include <utility>
#include <exception>

#include "cameraconfig.hpp"
#include "vector3.hpp"
#include "vector2.hpp"

Vector2 pnormalize_origin(Vector2 vec);

std::pair <Vector3,Vector3> build_vector(int camIdx, Vector3 cam_wpos, 
                                        Vector2 ball_ppos);

Vector3 closest_approach(const std::pair<Vector3, Vector3> l1,
                         const std::pair<Vector3, Vector3> l2);

Vector3 find_intersection(const std::pair<Vector3, Vector3> l1,
                            const std::pair<Vector3, Vector3> l2,
                            const std::pair<Vector3, Vector3> l3);

Vector3 find_intersection(const std::pair<Vector3, Vector3> l1,
                            const std::pair<Vector3, Vector3> l2);

#endif