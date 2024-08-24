//
// Implemented independently
//
#ifndef RAYTRACING_RANDOM_HPP
#define RAYTRACING_RANDOM_HPP

#include <random>
#include "Vector3f.h"
#include "Vector2f.h"

inline float rand01() {
    return rand() / (RAND_MAX + 1.0);
}

inline Vector3f randomUnitVector3d() {
    float a = rand01() * 2 * M_PI;
    float z = rand01() * 2 - 1;
    float r = sqrt(1 - z * z);
    return Vector3f(r * cos(a), r * sin(a), z);
}

inline Vector3f randomHemisphereDirection() {
    Vector3f v = randomUnitVector3d();
    if (v.z() < 0) v.z() = -v.z();

    return v;
}

inline Vector2f randomUnitVector2d() {
    float a = rand01() * 2 * M_PI;
    return Vector2f(cos(a), sin(a));
}

// Cosine Weighted Hemisphere Sampling
inline Vector3f randomCosineDirection() {
    float r1 = rand01();
    float r2 = rand01();
    float z = sqrt(1 - r2);
    float phi = 2 * M_PI * r1;
    float x = cos(phi) * 2 * sqrt(r2);
    float y = sin(phi) * 2 * sqrt(r2);
    return Vector3f(x, y, z);
}

inline Vector3f randomToSphere(float radius, float distance, const Vector3f &direction, float cosThetaMax) {
    float cosTheta = 1 - rand01() * (1 - cosThetaMax);
    float sinTheta = sqrt(1 - cosTheta * cosTheta);
    float phi = 2 * M_PI * rand01();
    float x = cos(phi) * sinTheta;
    float y = sin(phi) * sinTheta;
    return Vector3f(x, y, cosTheta);
}

#endif //RAYTRACING_RANDOM_HPP
