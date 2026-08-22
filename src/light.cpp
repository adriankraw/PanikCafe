#pragma once
#include <raylib.h>

class Light {
  public:
    Vector3 position{0,0,0};
    Vector3 direction{0,0,0};
    Vector3 Color{23.47f, 21.31f, 20.79f};
    Camera cam = Camera{0};
    float amplitude = 1;
    Matrix projectionMatrix{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    RenderTexture2D shadowTexture{0};
    RenderTexture2D shadowsCalculated{0};
    RenderTexture2D shadowsrendered{0};

    Light() {};
    Light(const Light&) = default;
    ~Light() {};
};
