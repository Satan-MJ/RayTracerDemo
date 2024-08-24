#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cmath>

#include "scene.hpp"
#include "camera.hpp"
#include "light.hpp"
#include "material.hpp"
#include "object3d.hpp"
#include "group.hpp"
#include "image.hpp"
#include "bvh_node.hpp"

#define DegreesToRadians(x) ((M_PI * x) / 180.0f)

Scene::Scene() {
    camera = nullptr;
    background_color = Vector3f(0, 0, 0);
    group = new Group();
    lights = new Group();
    bvh_root = nullptr;
}

Scene::~Scene() {
    delete group;
    delete camera;
    delete lights;
}

void Scene::addObject(Object3D *object) {
    group->addObject(object);
    if (object->material->isEmissive())
        lights->addObject(object);
}

void Scene::buildScene() {
    if (camera == nullptr) {
        printf("No camera specified\n");
        exit(0);
    }

    if (group->getGroupSize() == 0) {
        printf("No objects in the scene.\n");
        exit(0);
    }

    if (lights->getGroupSize() == 0) {
        printf("No lights in the scene.\n");
        exit(0);
    }

    bvh_root = new BVHNode(group->getObjects());
}
