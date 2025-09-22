#pragma once

#include <cstddef>
#include <cstdio>

#include "../scene.cpp"

#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>

class InputManager{
public:
    InputManager();
    InputManager(InputManager &&) = default;
    InputManager(const InputManager &) = default;
    InputManager &operator=(InputManager &&) = default;
    InputManager &operator=(const InputManager &) = default;
    ~InputManager();

    Scene* scene = nullptr;

    void Update();
    void ReadInput(Camera &camera);
    void ReadInput(Vector3 *lightPosition, Shader &coolShader);

private:
    
};

InputManager::InputManager() {
}

InputManager::~InputManager() {
}

void InputManager::Update(){
    ReadInput(scene->mainCam);
}

void InputManager::ReadInput(Camera &camera) {
    float mouseWheel = GetMouseWheelMove();
    camera.position += Vector3Scale(Vector3Normalize(camera.target - camera.position), mouseWheel / 10.0f);

    if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
        Vector2 mousePos = GetMouseDelta();
        camera.position = Vector3RotateByAxisAngle(camera.position, Vector3{0.0f, 1.0f, 0.0f}, -mousePos.x / 100.0f);
        camera.position = Vector3RotateByAxisAngle(
            camera.position, Vector3CrossProduct(camera.target - camera.position, camera.up), -mousePos.y / 100.0f);
    }
}

void InputManager::ReadInput(Vector3 *lightPosition, Shader &coolShader) {
    if (IsKeyDown(KEY_A)) {
        lightPosition->x += -5 * GetFrameTime();
        SetShaderValue(coolShader, rlGetLocationUniform(coolShader.id, "lightPos"), lightPosition, SHADER_UNIFORM_VEC3);
    }
    if (IsKeyDown(KEY_D)) {
        lightPosition->x += 5 * GetFrameTime();
        SetShaderValue(coolShader, rlGetLocationUniform(coolShader.id, "lightPos"), lightPosition, SHADER_UNIFORM_VEC3);
    }
    if (IsKeyDown(KEY_W)) {
        lightPosition->z += 5 * GetFrameTime();
        SetShaderValue(coolShader, rlGetLocationUniform(coolShader.id, "lightPos"), lightPosition, SHADER_UNIFORM_VEC3);
    }
    if (IsKeyDown(KEY_S)) {
        lightPosition->z += -5 * GetFrameTime();
        SetShaderValue(coolShader, rlGetLocationUniform(coolShader.id, "lightPos"), lightPosition, SHADER_UNIFORM_VEC3);
    }
    if (IsKeyDown(KEY_Q)) {
        lightPosition->y += 5 * GetFrameTime();
        SetShaderValue(coolShader, rlGetLocationUniform(coolShader.id, "lightPos"), lightPosition, SHADER_UNIFORM_VEC3);
    }
    if (IsKeyDown(KEY_E)) {
        lightPosition->y += -5 * GetFrameTime();
        SetShaderValue(coolShader, rlGetLocationUniform(coolShader.id, "lightPos"), lightPosition, SHADER_UNIFORM_VEC3);
    }
    if (IsKeyPressed(KEY_SPACE)) {
        printf("(%f/%f/%f)\n", lightPosition->x, lightPosition->y, lightPosition->z);
    }
}
