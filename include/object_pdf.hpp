//
// Referencing https://raytracing.github.io
//

#ifndef RAYTRACING_OBJECT_PDF_HPP
#define RAYTRACING_OBJECT_PDF_HPP

#include "pdf.hpp"

class Object3D;

class objectPdf : public pdf {
public:
    objectPdf(Object3D *p, const Vector3f &origin) : ptr(p), o(origin) {}

    float value(const Vector3f &direction) const override;
    Vector3f generate() const override;

private:
    Object3D *ptr;
    Vector3f o;
};

#endif //RAYTRACING_OBJECT_PDF_HPP
