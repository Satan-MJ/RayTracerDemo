//
// Referencing https://raytracing.github.io
//
#ifndef OBJECT3D_H
#define OBJECT3D_H

#include "ray.hpp"
#include "hit.hpp"
#include "material.hpp"
#include "aabb.hpp"

// Base class for all 3d entities.
class Object3D {
public:
    Object3D() : material(nullptr) {}

    virtual ~Object3D() = default;

    explicit Object3D(Material *material) {
        this->material = material;
    }

    // Intersect Ray with this object. If hit, store information in hit structure.
    virtual bool intersect(const Ray &r, Hit &h, float tmin) const = 0;

    virtual AABB getAABB() const = 0;

    virtual float pdfValue(const Vector3f &origin, const Vector3f &direction) const {
        return 0.0f;
    }

    virtual Vector3f random(const Vector3f &origin) const {
        return Vector3f(1, 0, 0);
    }

    Material *material;
};

#endif

