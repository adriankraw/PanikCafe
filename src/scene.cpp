#pragma once
#include "./gameObject.cpp"
#include "./assetManager/meshManager.cpp"
#include "./light.cpp"
#include <iostream>
#include <ostream>
#include <vector>
#include <rlgl.h>

class Scene {
public:
    Scene(std::string);
    ~Scene();
    void LoadScene(meshManager*);

    std::string name;
    Camera3D mainCam;
    Vector3 objPosition;
    std::vector<GameObject> gameObjects{};
    std::vector<Light> lights{};

    Model customersSpawnPoints{};

private:
    
};

Scene::Scene(std::string n):name{n}{
    gameObjects = {};

    mainCam = Camera3D{0};
    mainCam.position = Vector3{10, 10, 10};
    mainCam.target = Vector3{0, 0, 0};
    mainCam.up = Vector3{0, 1, 0};
    mainCam.fovy = 45.0f;
    mainCam.projection = CAMERA_PERSPECTIVE;

    objPosition = {0,0,0};

    lights.emplace_back(Light{});
    Camera3D *lightCam = &lights.back().cam;
    lightCam->position = Vector3{4.7f, 7.8f, 3.7f};
    lightCam->target = Vector3{0, 0, 0};
    lightCam->projection = CAMERA_ORTHOGRAPHIC;
    lightCam->up = Vector3{0, 1, 0};
    lightCam->fovy = 20.0f;

    lights.emplace_back(Light{});
    lightCam = &lights.back().cam;
    lightCam->position = Vector3{2.573f, 3.8829f, 6.510f};
    lightCam->target = Vector3{0, 0, 0};
    lightCam->projection = CAMERA_ORTHOGRAPHIC;
    lightCam->up = Vector3{0, 1, 0};
    lightCam->fovy = 20.0f;

    lights.emplace_back(Light{});
    lightCam = &lights.back().cam;
    lightCam->position = Vector3{7.303f, 3.883f, 6.510f};
    lightCam->target = Vector3{0, 0, 0};
    lightCam->projection = CAMERA_ORTHOGRAPHIC;
    lightCam->up = Vector3{0, 1, 0};
    lightCam->fovy = 20.0f;
}

Scene::~Scene() {
    for (auto &light : lights) {
        rlUnloadFramebuffer(light.shadowTexture.id);
    }
}

void Scene::LoadScene(meshManager *meshManager){
    for(int i = 0; i < meshManager->list.size(); ++i)
    {
        gameObjects.emplace_back(GameObject("empty", meshManager->list.at(i), Vector3{0,0,0}, Vector3{0,0,0}, Vector3{1,1,1}));
    }
    std::cout << name << ": " << gameObjects.size() << std::endl;
}
