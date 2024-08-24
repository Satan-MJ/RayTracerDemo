//
// Implemented independently
//

#ifndef RAYTRACING_MIXTURE_PDF_HPP
#define RAYTRACING_MIXTURE_PDF_HPP

#include "pdf.hpp"

class mixturePdf : public pdf {
public:
    /**
     * Construct a mixture of two pdfs
     * @param p1 first pdf
     * @param p2 second pdf
     * @param ratio ratio of the first pdf
     */
    mixturePdf(pdf *p1, pdf *p2, float ratio) : p1(p1), p2(p2), ratio(ratio) {}

    ~mixturePdf() override {
        delete p1;
        delete p2;
    }

    float value(const Vector3f &direction) const override {
        return ratio * p1->value(direction) + (1 - ratio) * p2->value(direction);
    }

    Vector3f generate() const override {
        if (rand01() < ratio) {
            return p1->generate();
        } else {
            return p2->generate();
        }
    }

    Vector3f generate(float& pdf) const override {
        if (rand01() < ratio) {
            return p1->generate(pdf);
        } else {
            return p2->generate(pdf);
        }
    }

private:
    pdf *p1, *p2;
    float ratio;
};

#endif //RAYTRACING_MIXTURE_PDF_HPP
