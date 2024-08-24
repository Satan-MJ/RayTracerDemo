//
// Referencing https://raytracing.github.io
//

#ifndef RAYTRACING_AABB_HPP
#define RAYTRACING_AABB_HPP

#include "interval.hpp"
#include "Vector3f.h"
#include "ray.hpp"

class AABB {
public:
    AABB() = default;

    AABB(const Interval &x, const Interval &y, const Interval &z) : x(x), y(y), z(z) {}

    AABB(const Vector3f &p1, const Vector3f &p2) {
        x = p1.x() < p2.x() ? Interval(p1.x(), p2.x()) : Interval(p2.x(), p1.x());
        y = p1.y() < p2.y() ? Interval(p1.y(), p2.y()) : Interval(p2.y(), p1.y());
        z = p1.z() < p2.z() ? Interval(p1.z(), p2.z()) : Interval(p2.z(), p1.z());
    }

    AABB(const AABB &aabb1, const AABB &aabb2) {
        x = Interval(aabb1.getX(), aabb2.getX());
        y = Interval(aabb1.getY(), aabb2.getY());
        z = Interval(aabb1.getZ(), aabb2.getZ());
    }

    void expand(const AABB &aabb) {
        x = Interval(x, aabb.getX());
        y = Interval(y, aabb.getY());
        z = Interval(z, aabb.getZ());
    }

    void expand(const Vector3f& p) {
        x = Interval(std::min(x.getMin(), p.x()), std::max(x.getMax(), p.x()));
        y = Interval(std::min(y.getMin(), p.y()), std::max(y.getMax(), p.y()));
        z = Interval(std::min(z.getMin(), p.z()), std::max(z.getMax(), p.z()));
    }

    Interval getX() const {
        return x;
    }

    Interval getY() const {
        return y;
    }

    Interval getZ() const {
        return z;
    }

    Interval getAxis(int axis) const {
        if (axis == 0) return x;
        if (axis == 1) return y;
        return z;
    }

    int getLongestAxis() const {
        float dx = x.getLength();
        float dy = y.getLength();
        float dz = z.getLength();
        if (dx > dy && dx > dz) return 0;
        if (dy > dz) return 1;
        return 2;
    }

    Vector3f getMin() const {
        return Vector3f(x.getMin(), y.getMin(), z.getMin());
    }

    Vector3f getMax() const {
        return Vector3f(x.getMax(), y.getMax(), z.getMax());
    }

    bool intersect(const Ray &r, float tmin, float tmax) const {
        float t1 = tmin, t2 = tmax;
        for (int i = 0; i < 3; i++) {
            float invD = 1.0f / r.getDirection()[i];
            float tNear = (getMin()[i] - r.getOrigin()[i]) * invD;
            float tFar = (getMax()[i] - r.getOrigin()[i]) * invD;
            if (tNear > tFar) std::swap(tNear, tFar);
            t1 = tNear > t1 ? tNear : t1;
            t2 = tFar < t2 ? tFar : t2;
            if (t1 > t2) return false;
        }
        return true;
    }
private:
    Interval x, y, z;
};

#endif //RAYTRACING_AABB_HPP
