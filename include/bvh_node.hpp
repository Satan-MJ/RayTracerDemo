//
// Referencing https://raytracing.github.io
//

#ifndef RAYTRACING_BVH_NODE_HPP
#define RAYTRACING_BVH_NODE_HPP

#include "object3d.hpp"
#include "group.hpp"

class BVHNode : public Object3D {
public:
    BVHNode() = delete;

    BVHNode(std::vector<Object3D*> objects) {
        aabb = AABB();
        for (auto object : objects) {
            aabb.expand(object->getAABB());
        }

        if (objects.size() == 1) {
            left = objects[0];
            right = nullptr;
        } else if (objects.size() == 2) {
            left = objects[0];
            right = objects[1];
        } else {
            int axis = aabb.getLongestAxis();
            std::sort(objects.begin(), objects.end(), [&axis](Object3D *a, Object3D *b) {
                return a->getAABB().getAxis(axis).getMax() < b->getAABB().getAxis(axis).getMax() ;
            });

            std::vector<Object3D*> left_objects(objects.begin(), objects.begin() + objects.size() / 2);
            std::vector<Object3D*> right_objects(objects.begin() + objects.size() / 2, objects.end());
            left = new BVHNode(left_objects);
            right = new BVHNode(right_objects);
        }
    }

    bool intersect(const Ray &r, Hit &h, float tmin) const override {
        if (!aabb.intersect(r, tmin, h.getT())) {
            return false;
        }
        bool hit_left = left->intersect(r, h, tmin);
        bool hit_right = right != nullptr && right->intersect(r, h, tmin);
        return hit_left || hit_right;
    }

    AABB getAABB() const override {
        return aabb;
    }

private:
    Object3D *left;
    Object3D *right;
    AABB aabb;
};

#endif //RAYTRACING_BVH_NODE_HPP
