#pragma once

#include <vector>
#include <string>

#include <raylib.h>

class GameObject {
  public:
    GameObject() {};
    GameObject(GameObject &&) = default;
    GameObject(const GameObject &) = default;
    GameObject &operator=(GameObject &&) = default;
    GameObject &operator=(const GameObject &) = default;
    ~GameObject() {};
    GameObject(std::string _name, Model _model, Vector3 _translation, Vector3 _rotation, Vector3 _scaling) {
        name = _name;
        model = _model;
        translation = _translation;
        rotation = _rotation;
        scaling = _scaling;
        animationCurrent = 0;
    }

    std::string name;
    Model model;
    Vector3 translation;
    Vector3 rotation;
    Vector3 scaling;
    std::vector<ModelAnimation> animations;
    float animationsFrame;
    int animationCurrent;
    bool depthIgnore = false;
};

