//
// Referencing PA1
//

#ifndef HIT_H
#define HIT_H

#include <vecmath.h>
#include "ray.hpp"

class Material;

class Hit {
public:

    // constructors
    Hit() {
        material = nullptr;
        t = 1e38;
    }

    Hit(float _t, Material *m, const Vector3f &n) {
        t = _t;
        material = m;
        normal = n;
    }

    Hit(const Hit &h) {
        t = h.t;
        material = h.material;
        normal = h.normal;
    }

    // destructor
    ~Hit() = default;

    float getT() const {
        return t;
    }

    Material *getMaterial() const {
        return material;
    }

    const Vector3f &getNormal() const {
        return normal;
    }

    float getU() const {
        return u;
    }

    float getV() const {
        return v;
    }

    void set(float _t, Material *m, const Vector3f &n) {
        t = _t;
        material = m;
        normal = n;
    }

    void setUV(float _u, float _v) {
        u = _u;
        v = _v;
    }

private:
    float t;
    Material *material;
    Vector3f normal;
    float u, v;     // texture coordinates
};

inline std::ostream &operator<<(std::ostream &os, const Hit &h) {
    os << "Hit <" << h.getT() << ", " << h.getNormal() << ">";
    return os;
}

#endif // HIT_H
