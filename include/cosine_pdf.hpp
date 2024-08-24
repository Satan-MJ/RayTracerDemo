//
// Referencing https://raytracing.github.io
//

#ifndef RAYTRACING_COSINE_PDF_HPP
#define RAYTRACING_COSINE_PDF_HPP

#include "pdf.hpp"

class cosinePdf : public pdf {
public:
    cosinePdf(const Vector3f &w) {
        uvw = orthonormalBasis(w);
    }

    float value(const Vector3f &direction) const override {
        float cosine = Vector3f::dot(direction.normalized(), uvw.getCol(2));
        return (cosine > 0) ? cosine / M_PI : 0;
    }

    Vector3f generate() const override {
        return uvw * randomCosineDirection();
    }

private:
    Matrix3f uvw;
};

#endif //RAYTRACING_COSINE_PDF_HPP
