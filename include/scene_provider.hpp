//
// Created by 麦家皓 on 2024/8/20.
//

#ifndef RAYTRACING_SCENE_PROVIDER_HPP
#define RAYTRACING_SCENE_PROVIDER_HPP

#include "scene.hpp"
#include "camera.hpp"
#include "material.hpp"
#include "texture.hpp"
#include "sphere.hpp"
#include "quad.hpp"
#include "plane.hpp"
#include "image.hpp"
#include "mesh.hpp"
#include "transform.hpp"
#include "surface.hpp"
#include "curve.hpp"

void setScene01(Scene &scene) {
    scene.setCamera(new PerspectiveCamera(Vector3f(0, 0, 10),
                                          Vector3f(0, 0, -1),
                                          Vector3f(0, 1, 0)));

    auto white = new ConstantTexture(Vector3f(1, 1, 1));
    auto gray = new ConstantTexture(0.05 * Vector3f(1, 1, 1));
    auto gold = new ConstantTexture(Vector3f(1, 0.886, 0.608));
    auto silver = new ConstantTexture(Vector3f(0.972, 0.960, 0.915));

    auto earthDiff = new ImageTexture(Image::LoadTGA("textures/earthmap.tga"));
    auto concreteDiff = new ImageTexture(Image::LoadTGA("textures/rough_concrete_diff_4k.tga"));
    auto concreteNor = new ImageTexture(Image::LoadTGA("textures/rough_concrete_nor_gl_4k.tga"));
    auto metalAlbedo = new ImageTexture(Image::LoadTGA("textures/Metal041A_4K-JPG_Color.tga"));
    auto metalRough = new ImageTexture(Image::LoadTGA("textures/Metal041A_4K-JPG_Roughness.tga"));
    auto metalMetal = new ImageTexture(Image::LoadTGA("textures/Metal041A_4K-JPG_Metalness.tga"));
    auto metalNormal = new ImageTexture(Image::LoadTGA("textures/Metal041A_4K-JPG_NormalGL.tga"));

    auto wallMaterial = new PrincipledSpecularMaterial(0, 0, 0, 0, 0, 0,
                                                       white, white, white, nullptr);
    auto specularMaterial = new PrincipledSpecularMaterial(0.5, 0, 0, 0.2, 0, 0,
                                                           white, white, white, nullptr);
    auto groundMaterial = new PrincipledSpecularMaterial(0, 0, 0, 0, 0, 0,
                                                         concreteDiff, white, white, concreteNor);
    auto metalMaterial = new GlossyMaterial(metalAlbedo, metalRough, metalMetal, metalNormal);
    scene.addObject(new Plane(Vector3f(0, 1, 0), -2.5, groundMaterial, 5)); // Ground
    scene.addObject(new Plane(Vector3f(0, -1, 0), -2.5, wallMaterial)); // Ceiling
    scene.addObject(new Plane(Vector3f(1, 0, 0), -2.5, specularMaterial)); // Left
    scene.addObject(new Plane(Vector3f(-1, 0, 0), -2.5, specularMaterial)); // Right
    scene.addObject(new Plane(Vector3f(0, 0, 1), -2.5, metalMaterial, 5)); // Front
    scene.addObject(new Plane(Vector3f(0, 0, -1), -12, wallMaterial)); // Back

    auto goldMaterial = new GlossyMaterial(gold, gray, white, nullptr);
    scene.addObject(new Sphere(Vector3f(-1, -1.5, 0), 1, goldMaterial));

    auto earthMaterial = new PrincipledSpecularMaterial(0.1, 0, 0, 0.3, 0, 0,
                                                        earthDiff, white, white, nullptr);
    scene.addObject(new Sphere(Vector3f(1.2, -1.7, 1), 0.8, earthMaterial,
                               Vector3f(-1, 0, 0.2), Vector3f(0, 1, 0)));

    auto silverMaterial = new GlossyMaterial(silver, gray, white, nullptr);
    scene.addObject(new Transform(new Mesh("mesh/cube.obj", silverMaterial),
                                  Vector3f(0.5, 2, 0.5), Vector3f(0.9, -0.5, -1.5), 0, 30, 0));

    auto areaLightMaterial = new PrincipledSpecularMaterial(0, 0, 1, 0, 0, 4,
                                                            white, white, white, nullptr);
    scene.addObject(new Quad(Vector3f(0, 2.499, 0), Vector3f(4, 0, 0), Vector3f(0, 0, 4), areaLightMaterial));

    auto airMaterial = new PrincipledSpecularMaterial(0, 1, 0, 0, 1, 0,
                                                      white, white, white, nullptr);
    auto glassMaterial = new PrincipledSpecularMaterial(0, 1, 0, 0, 1.5, 0,
                                                        white, white, white, nullptr);
    scene.addObject(new Sphere(Vector3f(0, -2, 2), 0.3, airMaterial));
    scene.addObject(new Sphere(Vector3f(0, -2, 2), 0.5, glassMaterial));
}

void setScene02(Scene &scene) {
    scene.setCamera(new PerspectiveCamera(Vector3f(5, 5, 10),
                                          Vector3f(-0.5, -0.5, -1),
                                          Vector3f(0, 1, 0),
                                          0.1, 10));

    auto white = new ConstantTexture(Vector3f(1, 1, 1));
    auto yellow = new ConstantTexture(Vector3f(1, 1, 0));
    auto magenta = new ConstantTexture(Vector3f(1, 0, 1));

    auto woodDiff = new ImageTexture(Image::LoadTGA("textures/wood_planks_diff_4k.tga"));
    auto woodNor = new ImageTexture(Image::LoadTGA("textures/wood_planks_nor_gl_4k.tga"));

    auto woodMaterial = new PrincipledSpecularMaterial(0, 0, 0, 0, 0, 0,
                                                       woodDiff, white, white, woodNor);
    auto yellowMaterial = new PrincipledSpecularMaterial(0, 0, 0, 0, 0, 0,
                                                         yellow, white, white, nullptr);
    auto magentaMaterial = new PrincipledSpecularMaterial(0, 0, 0, 0, 0, 0,
                                                          magenta, white, white, nullptr);
    auto emissiveMaterial = new PrincipledSpecularMaterial(0, 0, 1, 0, 0, 5,
                                                           white, white, white, nullptr);
    scene.addObject(new Plane(Vector3f(0, 1, 0), -1, woodMaterial)); // Ground
    scene.addObject(new Plane(Vector3f(0, -1, 0), -10.5, emissiveMaterial)); // Ceiling
    scene.addObject(new Plane(Vector3f(1, 0, 0), -6, magentaMaterial)); // Left
    scene.addObject(new Plane(Vector3f(-1, 0, 0), -10.5, emissiveMaterial)); // Right
    scene.addObject(new Plane(Vector3f(0, 0, 1), -6, yellowMaterial)); // Front
    scene.addObject(new Plane(Vector3f(0, 0, -1), -10.5, emissiveMaterial)); // Back

    auto glassMaterial = new PrincipledSpecularMaterial(0.3, 0.99, 0, 0, 1.33, 0,
                                                        white, white, white, nullptr);
    scene.addObject(new Sphere(Vector3f(1.5, -0.25, 0.5), 0.75, glassMaterial));
    scene.addObject(new Transform(new Mesh("mesh/bunny_200.obj", glassMaterial),
                                  6 * Vector3f(1, 1, 1), Vector3f(-0.5, -1, 0), 0, 0, 0));
}

void setScene03(Scene &scene) {
    scene.setCamera(new PerspectiveCamera(Vector3f(0, 0, 10),
                                          Vector3f(0, 0, -1),
                                          Vector3f(0, 1, 0),
                                          0, 10, 37));

    auto white = new ConstantTexture(Vector3f(1, 1, 1));
    auto gray5 = new ConstantTexture(0.5 * Vector3f(1, 1, 1));
    auto gray2 = new ConstantTexture(0.2 * Vector3f(1, 1, 1));
    auto gold = new ConstantTexture(Vector3f(1, 0.886, 0.608));
    auto silver = new ConstantTexture(Vector3f(0.972, 0.960, 0.915));

    auto goldMaterial = new GlossyMaterial(gold, gray2, white, nullptr);
    auto silverMaterial = new GlossyMaterial(silver, gray5, white, nullptr);
    auto emissiveMaterial = new PrincipledSpecularMaterial(0, 0, 1, 0, 0, 2.5,
                                                           white, white, white, nullptr);

    scene.addObject(new Quad(Vector3f(0, 2.499, 0), Vector3f(6, 0, 0), Vector3f(0, 0, 4), emissiveMaterial));
    scene.addObject(new Sphere(Vector3f(-0.5, -1.4, -0.5), 1, goldMaterial));

    auto metalAlbedo = new ImageTexture(Image::LoadTGA("textures/Metal041A_4K-JPG_Color.tga"));
    auto metalRough = new ImageTexture(Image::LoadTGA("textures/Metal041A_4K-JPG_Roughness.tga"));
    auto metalMetal = new ImageTexture(Image::LoadTGA("textures/Metal041A_4K-JPG_Metalness.tga"));
    auto metalNormal = new ImageTexture(Image::LoadTGA("textures/Metal041A_4K-JPG_NormalGL.tga"));
    auto metalMaterial = new GlossyMaterial(metalAlbedo, metalRough, metalMetal, metalNormal);

    auto bezierSurfaceGroup = BezierSurface::createBezierSurfaceGroup({
            {{-2, -2.5, -2}, {-1, -2.5, -2}, {1, -2.5, -2}, {2, -1.5, -2}},
            {{-2, -2.5, -1}, {-1, -2.5, -1}, {1, -2.5, -1}, {2, -2,   -1}},
            {{-2, -2.5, 1},  {-1, -2.5, 1},  {1, -2,   1},  {2, -1.5, 1}},
            {{-2, -2.5, 2},  {-1, -2.5, 2},  {1, -2.5, 2},  {2, -2.5, 2}},
            {{-2, -2.5, 3},  {-1, -2.5, 3},  {1, -3,   3},  {2, -3.5, 3}},
            {{-2, -2.5, 4},  {-1, -2.5, 4},  {1, -2.5, 4},  {2, -2.5, 4}},
            {{-2, -2.5, 5},  {-1, -2.5, 5},  {1, -2.5, 5},  {2, -2.5, 5}},
        }, metalMaterial);
    for(auto s : bezierSurfaceGroup) {
        scene.addObject(s);
    }

//    auto bezierCurveRev = BezierCurveRev::createBezierCurveRevGroup({
//        { 0, 2 },
//        { 1, 1.9999 },
//        { 1, 1 },
//        { 0, 0 },
//    }, goldMaterial);
//    scene.addObject(new Transform(bezierCurveRev, Vector3f(1, 1, 1),
//                                  Vector3f(-0.5, -1.4, -0.5), 90, 0, 0));
}

#endif //RAYTRACING_SCENE_PROVIDER_HPP
