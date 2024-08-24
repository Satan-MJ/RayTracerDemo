//
// Referencing PA1
//

#ifndef GROUP_H
#define GROUP_H


#include "object3d.hpp"
#include "ray.hpp"
#include "hit.hpp"
#include <iostream>
#include <vector>


class Group : public Object3D {

public:

    Group() = default;

    explicit Group (int num_objects) {
        objects.reserve(num_objects);
    }

    ~Group() override = default;

    bool intersect(const Ray &r, Hit &h, float tmin) const override {
        bool hit = false;
        for (auto obj : objects) {
            hit |= obj->intersect(r, h, tmin);
        }
        return hit;
    }

    void addObject(Object3D *obj) {
        objects.push_back(obj);
        aabb.expand(obj->getAABB());
    }

    int getGroupSize() {
        return objects.size();
    }

    Object3D* getObject(int index) {
        return objects[index];
    }

    std::vector<Object3D*> &getObjects() {
        return objects;
    }

    float pdfValue(const Vector3f &o, const Vector3f &v) const override {
        float sum = 0;
        for (auto obj : objects) {
            sum += obj->pdfValue(o, v);
        }
        return sum / objects.size();
    }

    Vector3f random(const Vector3f &o) const override {
        int index = rand() % objects.size();
        return objects[index]->random(o);
    }

    AABB getAABB() const override {
        return aabb;
    }
private:  
    std::vector<Object3D*> objects;
    AABB aabb;
};

#endif
	
