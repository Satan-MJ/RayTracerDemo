//
// Referencing PA1
//
#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>
#include <iostream>
using namespace std;


class Triangle: public Object3D {

public:
	Triangle() = delete;

    // a b c are three vertex positions of the triangle
	Triangle( const Vector3f& a, const Vector3f& b, const Vector3f& c, Material* m) : Object3D(m) {
		vertices[0] = a;
		vertices[1] = b;
		vertices[2] = c;
		normal = Vector3f::cross(b - a, c - a).normalized();

        aabb = AABB(a, b);
        aabb.expand(c);
	}

	bool intersect( const Ray& ray,  Hit& hit , float tmin) const override {
		Vector3f e1 = vertices[1] - vertices[0];
		Vector3f e2 = vertices[2] - vertices[0];
		Vector3f s = ray.getOrigin() - vertices[0];
		const Vector3f& d = ray.getDirection();

		Vector3f rd_e2 = Vector3f::cross(d, e2);
		Vector3f s_e1 = Vector3f::cross(s, e1);
		float deno = Vector3f::dot(rd_e2, e1);
		if (deno == 0) {
			return false;
		}

		float t = Vector3f::dot(s_e1, e2) / deno;
		float u = Vector3f::dot(rd_e2, s) / deno;
		float v = Vector3f::dot(s_e1, d) / deno;
		if (t > tmin && u >= 0 && v >= 0 && u + v <= 1) {
			if (t < hit.getT()) {
				hit.set(t, material, normal);
				return true;

			}
		}
		return false;
	}

    AABB getAABB() const override {
        return aabb;
    }

	Vector3f normal;
	Vector3f vertices[3];
protected:
    AABB aabb;
};

#endif //TRIANGLE_H
