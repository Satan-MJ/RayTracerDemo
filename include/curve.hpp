//
// Implemented independently
//
#ifndef RAYTRACING_CURVE_HPP
#define RAYTRACING_CURVE_HPP

#include "object3d.hpp"
#include "bernstein.hpp"

class BezierCurveRev : public Object3D {
public:
    // S_x = P_x * cos(theta), S_y = P_x * sin(theta), S_z = P_y
    // length of controls = 4
    explicit BezierCurveRev(std::vector<Vector2f> points, Material* m) : Object3D(m), controls(std::move(points)) {
        assert(controls.size() == 4);

        aabb = AABB();
        for (const auto &control : controls) {
            aabb.expand(Vector3f(control.x(), control.x(), control.y()));
            aabb.expand(Vector3f(-control.x(), -control.x(), control.y()));
        }
    }

    AABB getAABB() const override {
        return aabb;
    }

    bool intersect(const Ray &r, Hit &h, float tmin) const override {
        float u[] = {0.7, 0.3};
        bool result = false;
        for (float u_val : u) {
            result |= testIntersect(r, h, tmin, u_val);
        }
        return result;
    }

    bool testIntersect(const Ray &r, Hit &h, float tmin, float u) const {
        // solve F(u) = 0 using Newton's method
        for (int i = 0; i < 100; ++i) { // limit the number of iterations to prevent infinite loop
            Vector3f o = r.getOrigin();
            Vector3f d = r.getDirection().normalized();
            Vector2f p = evaluateBezierCurve(controls, u);
            Vector2f dp = evaluateBezierCurveDerivative(controls, u);
            float F_value = F(o, d, p);
            if (std::abs(F_value) < 1e-6) { // if F is small enough, we have found the intersection
                if (u < 0 || u > 1) {
                    return false;
                }

                // calculate normal
                float r_val = std::sqrt(p.x() * p.x() + p.y() * p.y());
                float dz_du = dp.y();

                float v = std::atan2(o.y() + r_val * d.y(), o.x() + r_val * d.x());
                Vector3f tangent_u(
                        dp.x() * std::cos(v),
                        dp.x() * std::sin(v),
                        dz_du
                );
                Vector3f tangent_v(
                        -r_val * std::sin(v),
                        r_val * std::cos(v),
                        0
                );

                Vector3f normal = Vector3f::cross(tangent_u, tangent_v).normalized();

                float t = (p.y() - o.z()) / d.z();
                if (t < tmin || t > h.getT()) {
                    return false;
                }

                h.set(t, material, normal);
                return true;
            }
            float F_derivative = FDerivative(o, d, p, dp);
            u -= F_value / F_derivative;
        }
        return false;
    }

    static std::vector<BezierCurveRev*> createBezierCurveRevGroup(const std::vector<Vector2f> &controls, Material *m) {
        assert(controls.size() >= 4 && controls.size() % 3 == 1);

        std::vector<BezierCurveRev*> group;
        for (int i = 0; i + 3 < controls.size(); i += 3) {
            std::vector<Vector2f> patch;
            patch.reserve(4);
            for (int j = 0; j < 4; ++j) {
                patch.push_back(controls[i + j]);
            }
            group.push_back(new BezierCurveRev(patch, m));
        }

        return group;
    }
private:
    std::vector<Vector2f> controls;
    AABB aabb;

    static float F(const Vector3f &o, const Vector3f &d, const Vector2f &p) {
        float x_cos = ((p.y() - o.z()) * d.x()) / d.z() + o.x();
        float x_sin = ((p.y() - o.z()) * d.y()) / d.z() + o.y();
        return x_cos * x_cos + x_sin * x_sin - p.x() * p.x();
    }

    static float FDerivative(const Vector3f &o, const Vector3f &d, const Vector2f &p, const Vector2f &dp) {
        float x_cos = ((p.y() - o.z()) * d.x()) / d.z() + o.x();
        float x_sin = ((p.y() - o.z()) * d.y()) / d.z() + o.y();
        float x_cos_derivative = 2 * x_cos * (d.x() / d.z() * dp.y());
        float x_sin_derivative = 2 * x_sin * (d.y() / d.z() * dp.y());
        return x_cos_derivative + x_sin_derivative - 2 * p.x() * dp.x();
    }

    static Vector2f evaluateBezierCurve(std::vector<Vector2f> controls, float u) {
        Vector2f result(0, 0);
        for (int i = 0; i < 4; ++i) {
            result += controls[i] * bernstein(3, i, u);
        }
        return result;
    }

    static Vector2f evaluateBezierCurveDerivative(std::vector<Vector2f> controls, float u) {
        Vector2f result(0, 0);
        for (int i = 0; i < 3; ++i) {
            result += (controls[i + 1] - controls[i]) * 3 * bernstein(2, i, u);
        }
        return result;
    }
};

#endif //RAYTRACING_CURVE_HPP
