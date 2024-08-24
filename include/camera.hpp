//
// Implemented independently
//
#ifndef CAMERA_H
#define CAMERA_H

#include "ray.hpp"
#include "random.hpp"
#include <vecmath.h>
#include <cmath>

#define DegreesToRadians(x) ((M_PI * x) / 180.0f)

class Camera {
public:
    Camera(const Vector3f &center, const Vector3f &direction, const Vector3f &up, int imgW, int imgH) {
        this->center = center;
        this->direction = direction.normalized();
        this->horizontal = Vector3f::cross(this->direction, up).normalized();
        this->up = Vector3f::cross(this->horizontal, this->direction);
        this->width = imgW;
        this->height = imgH;
    }

    // Generate rays for each screen-space coordinate
    virtual Ray generateRay(const Vector2f &point) = 0;
    virtual ~Camera() = default;

    int getWidth() const { return width; }
    int getHeight() const { return height; }

protected:
    // Extrinsic parameters
    Vector3f center;
    Vector3f direction;
    Vector3f up;
    Vector3f horizontal;
    // Intrinsic parameters
    int width;
    int height;
};


class PerspectiveCamera : public Camera {
public:
    PerspectiveCamera(const Vector3f &center, const Vector3f &direction, const Vector3f &up,
                      float aperture = 0, float focusDistance = 9.5, float angle = 30,
                      int imgW = 500, int imgH = 500)
            : Camera(center, direction, up, imgW, imgH), aperture(aperture) {
        // angle is in radian.
        scaleRatio = tan(DegreesToRadians(angle) / 2);

        auto imagePlaneWidth = 2 * focusDistance * scaleRatio;
        auto imagePlaneHeight = imagePlaneWidth * height / width;
        Vector3f imagePlaneAcross = this->horizontal * imagePlaneWidth;
        Vector3f imagePlaneDown = this->up * imagePlaneHeight;

        deltaX = imagePlaneAcross / width;
        deltaY = imagePlaneDown / height;

        pixelUpperLeft = center + focusDistance * direction - 0.5 * imagePlaneAcross - 0.5 * imagePlaneDown;
    }

    Ray generateRay(const Vector2f &point) override {
        // Perspective transformation
        Vector3f orig = aperture <= 0 ? center : sampleAperture();
        Vector3f pixelTarget = pixelUpperLeft + point.x() * deltaX + point.y() * deltaY;
        Vector3f dir = (pixelTarget - orig).normalized();

        return Ray(orig, dir);
    }
private:
    float scaleRatio;
    float aperture = 0.3;

    Vector3f deltaX, deltaY;
    Vector3f pixelUpperLeft;

    Vector3f sampleAperture() const {
        auto p = randomUnitVector2d();

        // translate to the aperture plane
        return center + p.x() * aperture * horizontal + p.y() * aperture * up;
    }
};

#endif //CAMERA_H
