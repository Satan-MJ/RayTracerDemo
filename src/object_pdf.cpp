//
// Referencing https://raytracing.github.io
//
#include "object_pdf.hpp"
#include "object3d.hpp"

float objectPdf::value(const Vector3f &direction) const {
    return ptr->pdfValue(o, direction);
}

Vector3f objectPdf::generate() const {
    return ptr->random(o);
}