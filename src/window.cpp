#include "./notif/notificationManager.cpp"
#include "./assetManager/meshManager.cpp"
#include "./assetManager/shaderManager.cpp"
#include "./config.cpp"
#include "./game.cpp"
#include "./renderer/meshRenderer.cpp"
#include "./renderer/uiRenderer.cpp"
#include "./scene.cpp"
#include <raylib.h>
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include <raymath.h>
#include <string>

class window {
  public:
    window();
    ~window();

    void Init(int, int, const std::string);
    void Loop();
    void Close();

  private:
    meshRenderer meshrenderer{};
    uiRenderer UIrendererer{};

    meshManager meshManager{};
    shaderManager shaderManager{};

    Game game{};
    Scene mainscene{"mainScene"};

};

window::window() {}

window::~window() {}


std::unique_ptr<NotificationManager> manager = std::unique_ptr<NotificationManager>(new NotificationManager());

void ReadInput(Camera &camera) {
    float mouseWheel = GetMouseWheelMove();
    camera.position += Vector3Scale(Vector3Normalize(camera.target - camera.position), mouseWheel / 10.0f);

    if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
        Vector2 mousePos = GetMouseDelta();
        camera.position = Vector3RotateByAxisAngle(camera.position, Vector3{0.0f, 1.0f, 0.0f}, -mousePos.x / 100.0f);
        camera.position = Vector3RotateByAxisAngle(
            camera.position, Vector3CrossProduct(camera.target - camera.position, camera.up), -mousePos.y / 100.0f);
    }
}

void ReadInput(Vector3 *lightPosition, Shader &coolShader) {
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
void window::Init(int width, int height, const std::string title) {
    InitWindow(width, height, title.c_str());
    config::getInstance().LoadConfig();

    game.scenes.push_back(mainscene);

    GetFontDefault() = config::getInstance().fontNormal;

    meshrenderer.game = &game;
    meshrenderer.scene = &game.scenes.at(0);
    UIrendererer.game = &game;

    meshrenderer.Init();
    UIrendererer.Init();

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    SetTargetFPS(60);

    float shadowMapResolution = 512 * 2;

    Vector3 viewPos = {0, 0, 0};
    Vector3 viewDir = {0, 0, 0};
    float rotation = 90;
    float uniformScale = 1.25f;
    float time = 0;

    meshManager.Import("./CafeBarRoom.glb");
    meshManager.Import("./CafeBar.glb");
    meshManager.Import("./CafeBar_Event_Ball.glb");
    meshManager.Import("./KitFox.glb");
    meshManager.Import("./Customers.glb");

    game.scenes[0].LoadScene(&meshManager);

    shaderManager.Import("./shadowmapVShader.glsl;./shadowmapFShader.glsl");
    meshrenderer.shader.emplace(renderer::RenderState::shadow,shaderManager.list.back());
    shaderManager.Import("./vertexShader.glsl;./fragmentShader.glsl");
    meshrenderer.shader.emplace(renderer::RenderState::light,shaderManager.list.back());
    shaderManager.Import(";./screenFragmentShader.glsl");
    meshrenderer.shader.emplace(renderer::RenderState::screen,shaderManager.list.back());
    UIrendererer.shader.emplace(renderer::RenderState::screen,shaderManager.list.back());

    meshrenderer.colorMap = LoadTexture("./texture/colorMap.png");

    //SetTraceLogLevel(LOG_WARNING);
}

void window::Loop() {
    Ray ray = {0};
    RayCollision collision = {0};

    meshrenderer.currentState = renderer::RenderState::shadow;
    meshrenderer.Loop();

    //UIrendererer.Loop();

    /*
    Light *lightCam = &game.scenes[0].lights[0];
    ReadInput(&lightCam->cam.position, meshrenderer.shader[renderer::RenderState::shadow]); // light movement WASD

    if (game.customersCount < game.customersMax) {
        game.customersCooldown -= GetFrameTime();
        if (game.customersCooldown <= 0) {
            game.customersCount++; // change this to spanCustomer();
            for (auto &cust : game.customers) {
                if (cust.active == false) {
                    cust.active = true;
                    cust.served = false;
                    game.SetupCustomer(&cust);
                    std::string order = (std::string)Enum_to_string(&cust.need_main);
                    if (cust.need_sup != CustomerNeed_Sup::Sup_None) {
                        order.append(" with ");
                        order.append(Enum_to_string(&cust.need_sup));
                    }
                    manager->AddNotification(NotificationEnum::ORDER, order);
                    break;
                }
            }
            game.customersCooldown = game.customersCooldownMax;
        }
    }



    // Raycast Mouseclick on customers
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (!collision.hit || true) {
            std::list<int> clickedLastFrame;
            int clickedThisFrame;
            ray = GetScreenToWorldRay(GetMousePosition(), game.scenes[0].mainCam);
            for (size_t cu = 0; cu < game.customers.size(); ++cu) {
                if (game.customers[cu].active) {
                    if (game.customers[cu].clicked) {
                        clickedLastFrame.push_back(cu);
                    }
                    collision = GetRayCollisionMesh(ray, *game.customers[cu].mesh, game.scenes[0].customersSpawnPoints.transform);
                    if (collision.hit) {
                        game.customers[cu].clicked = true;
                        clickedThisFrame = cu;
                        game.selectedCustomer = &game.customers[cu];
                    }
                }
            }
            if (clickedLastFrame.size() > 0) {
                for (int cu : clickedLastFrame) {
                    if (cu != clickedThisFrame) {
                        game.customers[cu].clicked = false;
                    }
                }
            }
        }
    }
    */
    manager->Update();
}

void window::Close() {

}
