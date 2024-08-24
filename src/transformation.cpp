//
// Implemented independently
//
#include "transformation.hpp"

Matrix3f orthonormalBasis(const Vector3f &w)  {
    Vector3f unit_w = w.normalized();
    Vector3f a = (fabs(unit_w.x()) > 0.9) ? Vector3f(0, 1, 0) : Vector3f(1, 0, 0);
    Vector3f v = Vector3f::cross(unit_w, a).normalized();
    Vector3f u = Vector3f::cross(unit_w, v);
    return Matrix3f(u, v, unit_w, true);
}