//
// Referencing PA1
//
#ifndef PLANE_H
#define PLANE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>

// Infinite plane
// function: ax+by+cz=d
// choose your representation , add more fields and fill in the functions

class Plane : public Object3D {
public:
    Plane() = delete;

    Plane(const Vector3f &normal, float d, Material *m, float uvScale = 1) : Object3D(m), normal(normal), d(d), uvScale(uvScale) {
        basis1 = Vector3f(0, 1, 0);
        if (fabs(Vector3f::dot(basis1, normal)) > 0.9) {
            basis1 = Vector3f(1, 0, 0);
        }
        basis1 = Vector3f::cross(basis1, normal).normalized();
        basis2 = Vector3f::cross(normal, basis1).normalized();
        TBNRotation = Matrix3f(basis1, basis2, normal);

        if (normal.x() == 0 && normal.y() == 0) {
            float z = d / normal.z();
            aabb = AABB(Interval::Full(), Interval::Full(), Interval(z, z));
        }
        else if (normal.x() == 0 && normal.z() == 0) {
            float y = d / normal.y();
            aabb = AABB(Interval::Full(), Interval(y, y), Interval::Full());
        }
        else if (normal.y() == 0 && normal.z() == 0) {
            float x = d / normal.x();
            aabb = AABB(Interval(x, x), Interval::Full(), Interval::Full());
        }
        else {
            aabb = AABB(Interval::Full(), Interval::Full(), Interval::Full());
        }
    }

    ~Plane() override = default;

    bool intersect(const Ray &r, Hit &h, float tmin) const override {
        float t = (d - Vector3f::dot(normal, r.getOrigin())) / Vector3f::dot(normal, r.getDirection());
        if (t > tmin && t < h.getT()) {
            Vector3f p = r.pointAtParameter(t);
            float u, v;

            Vector3f p0 = p - normal * d;
            u = Vector3f::dot(p0, basis1);
            v = Vector3f::dot(p0, basis2);

            // scale the uv
            u /= uvScale;
            v /= uvScale;
            u -= floor(u);
            v -= floor(v);

            h.set(t, material, normal);
            h.setUV(u, v);
            return true;
        }
        return false;
    }

    AABB getAABB() const override {
        return aabb;
    }

    Vector3f normal;
    float d;
protected:
    Vector3f basis1, basis2;
    Matrix3f TBNRotation;
    float uvScale;

    AABB aabb;
};

#endif //PLANE_H
		

