//
// Referencing https://raytracing.github.io
//
#ifndef RAYTRACING_CONSTANT_MEDIUM_HPP
#define RAYTRACING_CONSTANT_MEDIUM_HPP

#include "object3d.hpp"

class ConstantMedium : public Object3D {
public:
    ConstantMedium(Object3D *boundary, float density, const Vector3f& color)
            : boundary(boundary), negInvDensity(-1/density), phaseFunction(new IsotropicMaterial(new ConstantTexture(color))) {}

    ~ConstantMedium() override {
        delete boundary;
        delete phaseFunction;
    }

    bool intersect(const Ray &r, Hit &h, float tmin) const override {
        Hit h1, h2;
        if (!boundary->intersect(r, h1, tmin))
            return false;
        if (!boundary->intersect(r, h2, h1.getT() + 0.0001f))
            return false;

        auto h1T = h1.getT();
        auto h2T = h2.getT();
        if (h1T < tmin) h1T = tmin;
        if (h2T > h.getT()) h2T = h.getT();
        if (h1T >= h2T)
            return false;

        if (h1T < 0) h1T = 0;

        float distanceInsideBoundary = (h2T - h1T) * r.getDirection().length();
        float hitDistance = negInvDensity * log(drand48());
        if (hitDistance < distanceInsideBoundary) {
            h.set(h1T + hitDistance / r.getDirection().length(), phaseFunction, Vector3f(1, 0, 0));
            return true;
        }
        return false;
    }

    AABB getAABB() const override {
        return boundary->getAABB();
    }

private:
    Object3D *boundary;
    float negInvDensity;
    Material *phaseFunction;
};

#endif //RAYTRACING_CONSTANT_MEDIUM_HPP
