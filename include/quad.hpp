//
// Implemented independently
//

#ifndef RAYTRACING_QUAD_HPP
#define RAYTRACING_QUAD_HPP

#include "object3d.hpp"

class Quad : public Object3D {
public:
    Quad(const Vector3f &center, const Vector3f &a, const Vector3f &b, Material *m)
            : Object3D(m), a(a), b(b) {
        normal = Vector3f::cross(a, b).normalized();
        upperLeft = center - a / 2 - b / 2;

        Vector3f bottomRight = upperLeft + a + b;
        aabb = AABB(upperLeft, bottomRight);
    }

    ~Quad() override = default;

    bool intersect(const Ray &r, Hit &h, float tmin) const override {
        auto denominator = Vector3f::dot(normal, r.getDirection());
        if (fabs(denominator) < 1e-6) return false;

        auto t = Vector3f::dot(upperLeft - r.getOrigin(), normal) / denominator;
        if (t < tmin || t > h.getT()) return false;

        auto p = r.pointAtParameter(t);
        auto d = p - upperLeft;
        auto ddota = Vector3f::dot(d, a);
        auto ddotb = Vector3f::dot(d, b);
        if (ddota < 0 || ddota > a.squaredLength() || ddotb < 0 || ddotb > b.squaredLength()) return false;

        h.set(t, material, normal);
        return true;
    }

    float pdfValue(const Vector3f &o, const Vector3f &v) const override {
        Hit h;
        if (intersect(Ray(o, v), h, 0.001f)) {
            float area = Vector3f::cross(a, b).length();
            float distance_squared = h.getT() * h.getT() * v.squaredLength();
            float cosine = fabs(Vector3f::dot(v.normalized(), h.getNormal()));
            return distance_squared / (cosine * area);
        } else {
            return 0;
        }
    }

    Vector3f random(const Vector3f &origin) const override {
        Vector3f randomPoint = upperLeft + a * rand01() + b * rand01();
        return randomPoint - origin;
    }

    AABB getAABB() const override {
        return aabb;
    }
private:
    Vector3f upperLeft, a, b, normal;
    AABB aabb;
};

#endif //RAYTRACING_QUAD_HPP
