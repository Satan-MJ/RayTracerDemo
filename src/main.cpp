//
// main function copied from PA1
// Other parts implemented independently
//
#include <cmath>
#include <iostream>
#include <thread>
#include <string>

#include "scene.hpp"
#include "image.hpp"
#include "camera.hpp"
#include "group.hpp"
#include "light.hpp"
#include "random.hpp"
#include "bvh_node.hpp"
#include "scene_provider.hpp"

using namespace std;

Vector3f trace(const Ray &ray, Scene *scene, Object3D* lights, int depth) {
    Hit hit;
    bool intersect = scene->getBVHRoot()->intersect(ray, hit, 0);
    if (!intersect) {
        return scene->getBackgroundColor();
    }

    auto* material = hit.getMaterial();

    Vector3f attenuation;
    Ray scattered(Vector3f(0), Vector3f(0));
    Vector3f emmision = material->scatter(ray, hit, attenuation, scattered, lights);

    // This has to come first
    if (emmision != Vector3f::ZERO)
        return emmision;

    // Russian Roulette
    if (attenuation.length() < rand01())
        return Vector3f::ZERO;

    // Prevent stack overflow
    if (depth >= 50)
        return emmision;

    Vector3f finalColor = emmision
            + attenuation * trace(scattered, scene, lights, depth + 1);
    return finalColor;
}

const int SAMPLE_LIMIT = 1000;

Vector3f tracePixelTask(const std::tuple<int, int, Scene*, Camera*, int> *arg) {
    int i = std::get<0>(*arg);
    int j = std::get<1>(*arg);
    Scene* scene = std::get<2>(*arg);
    Camera* camera = std::get<3>(*arg);
    int samples = std::get<4>(*arg);

    Vector3f color;
    for (int k = 0; k < samples; k++) {
        float u = i + rand01();
        float v = j + rand01();
        Ray ray = camera->generateRay(Vector2f(u, v));

        Vector3f newColor = trace(ray, scene, scene->getLights(), 0);

        if(newColor.x() != newColor.x() || newColor.y() != newColor.y() || newColor.z() != newColor.z()) {
            samples--;
            continue;
        }

        float l = newColor.length();
        if(l > 100) {
            newColor = newColor / l * color.length();
        }
        color += newColor;
    }
    color = color / samples;
    return color;
}

int main(int argc, char *argv[]) {
    for (int argNum = 1; argNum < argc; ++argNum) {
        std::cout << "Argument " << argNum << " is: " << argv[argNum] << std::endl;
    }

    if (argc != 3) {
        cout << "Usage: ./bin/PA1 <output bmp file> <number of threads>" << endl;
        return 1;
    }
    string outputFile = argv[1];  // only bmp is allowed.
    int numThreads = stoi(argv[2]);

    // Main RayCasting Logic
    // Loop over each pixel in the image, shooting a ray
    // through that pixel and finding its intersection with
    // the scene.  Write the color at the intersection to that
    // pixel in your output image.
    Scene scene;
    setScene03(scene);
    scene.buildScene();

    Camera *camera = scene.getCamera();
    Image image(camera->getWidth(), camera->getHeight());

    std::thread threads[numThreads];

    bool firstRound = true;
    int samples = 16;
    while (samples < SAMPLE_LIMIT) {
        for (int i = 0; i < camera->getWidth(); i++) {
            std::cout << samples << " Rendering: " << i << " / " << camera->getWidth() << std::endl;
            for (int j = 0; j < camera->getHeight(); j++) {
                std::tuple<int, int, Scene*, Camera*, int> args(i, j, &scene, camera, samples);
                if(threads[j % numThreads].joinable())
                    threads[j % numThreads].join();
                threads[j % numThreads] = std::thread([i, j, &image, args]() {
                    Vector3f color = tracePixelTask(&args);
                    for (int k = 0; k < 3; k++)
                        color[k] = pow(color[k], 1.0f / 2.2f);  // Gamma correction (inverse gamma correction)

                    image.SetPixel(i, j, image.GetPixel(i, j) / 2.0f + color / 2.0f);
                });
            }
        }
        for (auto &thread : threads) {
            if (thread.joinable())
                thread.join();
        }

        if(!firstRound)
            samples <<= 1;
        else
            firstRound = false;

        auto filename = outputFile.substr(0, outputFile.find_last_of('.')) + "-" + std::to_string(samples) + ".bmp";
        image.SaveImage(filename.c_str());
    }

    std::cout << "Done" << std::endl;
    image.SaveImage(outputFile.c_str());

    return 0;
}

