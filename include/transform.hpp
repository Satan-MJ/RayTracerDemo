//
// Referencing PA1
//
#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <vecmath.h>
#include "object3d.hpp"

#define DegreesToRadians(x) ((M_PI * x) / 180.0f)

// transforms a 3D point using a matrix, returning a 3D point
static Vector3f transformPoint(const Matrix4f &mat, const Vector3f &point) {
    return (mat * Vector4f(point, 1)).xyz();
}

// transform a 3D directino using a matrix, returning a direction
static Vector3f transformDirection(const Matrix4f &mat, const Vector3f &dir) {
    return (mat * Vector4f(dir, 0)).xyz();
}

class Transform : public Object3D {
public:
    Transform() = delete;

    Transform(const Matrix4f &m, Object3D *obj) : o(obj), Object3D(obj->material) {
        transform = m.inverse();

        setNewAABB(m, o->getAABB());
    }

    Transform(Object3D* obj, const Vector3f &scale, const Vector3f &translate, float rotateX, float rotateY, float rotateZ)
        : o(obj), Object3D(obj->material)  {
        Matrix4f m = Matrix4f::identity();

        m = m * Matrix4f::translation(translate);
        m = m * Matrix4f::rotateX(DegreesToRadians(rotateX));
        m = m * Matrix4f::rotateY(DegreesToRadians(rotateY));
        m = m * Matrix4f::rotateZ(DegreesToRadians(rotateZ));
        m = m * Matrix4f::scaling(scale.x(), scale.y(), scale.z());

        transform = m.inverse();

        setNewAABB(m, o->getAABB());
    }

    ~Transform() {
        delete o;
    }

    virtual bool intersect(const Ray &r, Hit &h, float tmin) const override {
        Vector3f trSource = transformPoint(transform, r.getOrigin());
        Vector3f trDirection = transformDirection(transform, r.getDirection());
        Ray tr(trSource, trDirection);
        bool inter = o->intersect(tr, h, tmin);
        if (inter) {
            h.set(h.getT(), h.getMaterial(), transformDirection(transform.transposed(), h.getNormal()).normalized());
        }
        return inter;
    }

    AABB getAABB() const override {
        return newAABB;
    }

protected:
    Object3D *o; //un-transformed object
    Matrix4f transform;
    AABB newAABB;

    void setNewAABB(const Matrix4f &m, const AABB &aabb) {
        Vector3f min = aabb.getMin(), max = aabb.getMax();
        Vector3f vertices[8] = {
                Vector3f(min.x(), min.y(), min.z()),
                Vector3f(min.x(), min.y(), max.z()),
                Vector3f(min.x(), max.y(), min.z()),
                Vector3f(min.x(), max.y(), max.z()),
                Vector3f(max.x(), min.y(), min.z()),
                Vector3f(max.x(), min.y(), max.z()),
                Vector3f(max.x(), max.y(), min.z()),
                Vector3f(max.x(), max.y(), max.z())
        };
        for (auto & vertex : vertices) {
            vertex = transformPoint(m, vertex);
            newAABB.expand(vertex);
        }
    }
};

#endif //TRANSFORM_H
