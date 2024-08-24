//
// Implemented independently
//

#ifndef RAYTRACING_SURFACE_HPP
#define RAYTRACING_SURFACE_HPP

#include <tuple>
#include <vector>
#include "Vector3f.h"
#include "object3d.hpp"
#include "bernstein.hpp"

class BezierSurface : public Object3D {
public:
    explicit BezierSurface(std::vector<std::vector<Vector3f>> points, Material* m) : controls(std::move(points)), Object3D(m) {
        aabb = AABB();
        for (const auto &control : controls) {
            for (const auto &point : control) {
                aabb.expand(point);
            }
        }
    }

    std::vector<std::vector<Vector3f>> &getControls() {
        return controls;
    }

    AABB getAABB() const override {
        return aabb;
    }

    bool intersect(const Ray &r, Hit &h, float tmin) const override {
        // solve L(t) - P(u, v) = 0 using Newton's method
        float t = 0, u = 0.5, v = 0.5;
        Vector3f F;
        std::vector<float> delta;
        Matrix3f J;
        for (int i = 0; i < 100; ++i) { // limit the number of iterations to prevent infinite loop
            Vector3f L = r.pointAtParameter(t);
            Vector3f P = evaluateBezierSurface(controls, u, v);
            F = L - P;
            if (F.length() < 1e-6) { // if F is small enough, we have found the intersection
                if (t < tmin || t > h.getT() || u < 0 || u > 1 || v < 0 || v > 1) {
                    return false;
                }
                Vector3f normal = Vector3f::cross(evaluateBezierSurfaceDerivativeU(controls, u, v),
                                                  evaluateBezierSurfaceDerivativeV(controls, u, v)).normalized();

                h.set(t, material, normal);
                h.setUV(u, v);
                return true;
            }
            Vector3f dLdt = r.getDirection();
            Vector3f dPdu = evaluateBezierSurfaceDerivativeU(controls, u, v);
            Vector3f dPdv = evaluateBezierSurfaceDerivativeV(controls, u, v);
            J = Matrix3f(dLdt, -dPdu, -dPdv);
            delta = gaussianElimination(J, F); // solve J * delta = -F
            t += delta[0];
            u += delta[1];
            v += delta[2];
        }
        return false;
    }

    static std::vector<BezierSurface*> createBezierSurfaceGroup(const std::vector<std::vector<Vector3f>> &controls, Material *m) {
        std::vector<BezierSurface*> group;

        assert(controls.size() >= 4 && controls.size() % 3 == 1);
        for (const auto &row : controls) {
            assert(row.size() >= 4 && row.size() % 3 == 1);
        }

        for (int i = 0; i + 3 < controls.size(); i += 3) {
            for (int j = 0; j + 3 < controls[i].size(); j += 3) {
                std::vector<std::vector<Vector3f>> patch;
                patch.reserve(4);
                for (int ii = 0; ii < 4; ++ii) {
                    std::vector<Vector3f> row;
                    row.reserve(4);
                    for (int jj = 0; jj < 4; ++jj) {
                        row.push_back(controls[i + ii][j + jj]);
                    }
                    patch.push_back(row);
                }
                group.push_back(new BezierSurface(patch, m));
            }
        }

        return group;
    }

private:
    std::vector<std::vector<Vector3f>> controls;
    AABB aabb;

    static Vector3f evaluateBezierSurface(const std::vector<std::vector<Vector3f>> &controls, float u, float v) {
        Vector3f P(0, 0, 0);
        for (int i = 0; i <= 3; ++i) {
            for (int j = 0; j <= 3; ++j) {
                P += bernstein(3, i, u) * bernstein(3, j, v) * controls[i][j];
            }
        }
        return P;
    }

    static Vector3f evaluateBezierSurfaceDerivativeU(const std::vector<std::vector<Vector3f>> &controls, float u, float v) {
        Vector3f dPdu(0, 0, 0);
        for (int i = 0; i <= 3; ++i) {
            for (int j = 0; j <= 3; ++j) {
                dPdu += bernsteinDerivative(3, i, u) * bernstein(3, j, v) * controls[i][j];
            }
        }
        return dPdu;
    }

    static Vector3f evaluateBezierSurfaceDerivativeV(const std::vector<std::vector<Vector3f>> &controls, float u, float v) {
        Vector3f dPdv(0, 0, 0);
        for (int i = 0; i <= 3; ++i) {
            for (int j = 0; j <= 3; ++j) {
                dPdv += bernstein(3, i, u) * bernsteinDerivative(3, j, v) * controls[i][j];
            }
        }
        return dPdv;
    }

    static std::vector<float> gaussianElimination(Matrix3f J, Vector3f F) {
        int n = 3;
        std::vector<std::vector<float>> a(n, std::vector<float>(n + 1));

        // Fill the augmented matrix
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                a[i][j] = J(i, j);
            }
            a[i][n] = -F[i];
        }

        // Gaussian elimination
        for (int i = 0; i < n; ++i) {
            // Find the row with the maximum value in column i
            int maxRow = i;
            for (int j = i + 1; j < n; ++j) {
                if (abs(a[j][i]) > abs(a[maxRow][i])) {
                    maxRow = j;
                }
            }

            // Swap rows i and maxRow
            std::swap(a[i], a[maxRow]);

            // Normalize row i
            for (int j = i + 1; j <= n; ++j) {
                a[i][j] /= a[i][i];
            }

            // Reduce other rows
            for (int j = 0; j < n; ++j) {
                if (j != i) {
                    for (int k = i + 1; k <= n; ++k) {
                        a[j][k] -= a[i][k] * a[j][i];
                    }
                }
            }
        }

        // Extract the solution
        std::vector<float> solution(n);
        for (int i = 0; i < n; ++i) {
            solution[i] = a[i][n];
        }

        return solution;
    }
};

#endif //RAYTRACING_SURFACE_HPP
