//
// Implemented independently
//

#ifndef RAYTRACING_TEXTURE_HPP
#define RAYTRACING_TEXTURE_HPP

#include <algorithm>
#include "Vector3f.h"
#include "image.hpp"

class Texture {
public:
    virtual Vector3f getColor(float u, float v) const = 0;
    virtual ~Texture() = default;
};

class ConstantTexture : public Texture {
public:
    explicit ConstantTexture(const Vector3f &color) : color(color) {}

    Vector3f getColor(float u, float v) const override {
        return color;
    }

private:
    Vector3f color;
};

class ImageTexture : public Texture {
public:
    explicit ImageTexture(Image *image)
        : image(image) {}

    ~ImageTexture() override {
        delete image;
    }

    Image* getImage() const {
        return image;
    }

    Vector3f getColor(float u, float v) const override {
        u = std::max(0.0f, std::min(1.0f, u));
        v = std::max(0.0f, std::min(1.0f, v));
        int x = (int)(u * image->Width());
        int y = (int)(v * image->Height());

        if (x == image->Width())
            x--;
        if (y == image->Height())
            y--;
        return image->GetPixel(x, y);
    }

private:
    Image* image;
};

#endif //RAYTRACING_TEXTURE_HPP
