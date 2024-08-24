//
// Referencing PA1
//
#ifndef SPHERE_H
#define SPHERE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>

class Sphere : public Object3D {
public:
    Sphere() = delete;

    Sphere(const Vector3f &center, float radius, Material *material, Matrix3f *uvRotation = nullptr)
        : Object3D(material), center(center), radius(radius), rotation(uvRotation == nullptr ? Matrix3f::identity() : *uvRotation) {
        Vector3f r = Vector3f(radius, radius, radius);
        aabb = AABB(center - r, center + r);
    }

    Sphere(const Vector3f &center, float radius, Material *material, Vector3f uvXAxis, Vector3f uvYAxis)
        : Sphere(center, radius, material) {
        uvXAxis.normalize();
        uvYAxis.normalize();
        assert(fabs(Vector3f::dot(uvXAxis, uvYAxis)) < 0.2f);

        Vector3f uvZAxis = Vector3f::cross(uvXAxis, uvYAxis).normalized();
        uvXAxis = Vector3f::cross(uvYAxis, uvZAxis).normalized();
        rotation = Matrix3f(uvXAxis, uvYAxis, uvZAxis);
    }

    ~Sphere() override = default;

    bool intersect(const Ray &r, Hit &h, float tmin) const override {
        Ray rayNormalized(r.getOrigin(), r.getDirection().normalized());

        Vector3f l = center - rayNormalized.getOrigin();
        float l_sq = l.squaredLength();
        float r_sq = radius * radius;

        float tp = Vector3f::dot(l, rayNormalized.getDirection());
        if (tp < 0 && l_sq < r_sq) return false;
        
        float d_sq = l_sq - tp * tp;
        if (d_sq > r_sq) return false;

        float t_dash = sqrt(r_sq - d_sq);

        float t = tp - t_dash;

        if (t <= tmin || t >= h.getT()) {
            t = tp + t_dash;
            if (t <= tmin || t >= h.getT()) {
                return false;
            }
        }

        Vector3f outwardNormal = (rayNormalized.pointAtParameter(t) - center).normalized();
        h.set(t, material, outwardNormal);

        float u, v;
        getSphereUV(outwardNormal, u, v);
        h.setUV(u, v);

        return true;
    }

    Vector3f random(const Vector3f &origin) const override {
        Vector3f direction = center - origin;
        float distance = direction.length();
        direction.normalize();
        float cosThetaMax = sqrt(1 - radius * radius / distance / distance);
        return randomToSphere(radius, distance, direction, cosThetaMax);
    }

    float pdfValue(const Vector3f &origin, const Vector3f &direction) const override {
        Hit h;
        if (!intersect(Ray(origin, direction), h, 0.001f)) {
            return 0;
        }

        float cosThetaMax = sqrt(1 - radius * radius / (center - origin).squaredLength());
        float solidAngle = 2 * M_PI * (1 - cosThetaMax);
        return 1 / solidAngle;
    }

    AABB getAABB() const override {
        return aabb;
    }

protected:
    Vector3f center;
    float radius;
    Matrix3f rotation;
    AABB aabb;

    void getSphereUV(const Vector3f &p, float &u, float &v) const {
        // p is a point on the unit sphere
        // u,v is the texture coordinate
        Vector3f rotatedP = rotation * p;

        float phi = atan2(rotatedP.z(), rotatedP.x());
        float theta = asin(rotatedP.y());
        u = 1 - (phi + M_PI) / (2 * M_PI);
        v = (theta + M_PI / 2) / M_PI;
    }
};


#endif
