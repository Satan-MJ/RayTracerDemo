//
// Referencing PA1
//
#ifndef SCENE_PARSER_H
#define SCENE_PARSER_H

#include <cassert>
#include <vecmath.h>
#include <vector>

class Camera;
class Light;
class Material;
class Object3D;
class Group;
class BVHNode;

class Scene {
public:

    Scene();

    ~Scene();

    void buildScene();

    /* Getters */

    Camera *getCamera() const {
        return camera;
    }

    Vector3f getBackgroundColor() const {
        return background_color;
    }

    Group *getLights() const {
        return lights;
    }

    Group *getGroup() const {
        return group;
    }

    BVHNode *getBVHRoot() const {
        return bvh_root;
    }

    /* Setters */

    void setCamera(Camera *cam) {
        camera = cam;
    }

    void setBackgroundColor(const Vector3f &color) {
        background_color = color;
    }

    void addObject(Object3D *object);

private:

    Camera *camera;
    Vector3f background_color;
    Group *lights;
    Group *group;
    BVHNode *bvh_root;
};

#endif // SCENE_PARSER_H
