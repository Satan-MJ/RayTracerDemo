//
// Referencing https://raytracing.github.io
//

#ifndef RAYTRACING_PDF_HPP
#define RAYTRACING_PDF_HPP

#include <cmath>
#include "Vector3f.h"
#include "random.hpp"
#include "Matrix3f.h"
#include "transformation.hpp"

class pdf {
public:
    virtual ~pdf() = default;

    virtual float value(const Vector3f &direction) const = 0;
    virtual Vector3f generate() const = 0;

    virtual Vector3f generate(float& pdf) const {
        Vector3f dir = generate();
        pdf = value(dir);
        return dir;
    }
};

class spherePdf : public pdf {
public:
    spherePdf() = default;

    float value(const Vector3f &direction) const override {
        return 1.0f / (4.0f * M_PI);
    }

    Vector3f generate() const override {
        return randomUnitVector3d();
    }
};

#endif //RAYTRACING_PDF_HPP
