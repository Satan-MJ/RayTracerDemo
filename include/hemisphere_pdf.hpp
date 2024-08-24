//
// Implemented independently
//

#ifndef RAYTRACING_HEMISPHERE_PDF_HPP
#define RAYTRACING_HEMISPHERE_PDF_HPP

#include "pdf.hpp"

class hemispherePdf : public pdf {
public:
    explicit hemispherePdf(const Vector3f &w) {
        uvw = orthonormalBasis(w);
    }

    float value(const Vector3f &direction) const override {
        return 1 / (2 * M_PI);
    }

    Vector3f generate() const override {
        return uvw * randomHemisphereDirection();
    }

private:
    Matrix3f uvw;
};

#endif //RAYTRACING_HEMISPHERE_PDF_HPP
