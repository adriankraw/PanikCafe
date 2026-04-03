#pragma once
#include "./notif/notificationManager.cpp"
#include "./assetManager/meshManager.cpp"
#include "./assetManager/shaderManager.cpp"
#include "./config.cpp"
#include "./game.cpp"
#include "./renderer/meshRenderer.cpp"
#include "./renderer/uiRenderer.cpp"
#include "./renderer/imguiRenderer.cpp"
#include "./scene.cpp"
#include "./input/inputManager.cpp"

#include "./imgui/imgui.h"

#include <raylib.h>
#define RAYGUI_IMPLEMENTATION
#include <raymath.h>

#include <string>

class window {
  public:
    window();
    ~window();

    void Init(int width, int height, const std::string title);
    void Loop();
    void Close();

  private:
    meshRenderer meshrenderer{};
    uiRenderer UIrendererer{};
    imguiRenderer imguiRenderer{};

    meshManager meshManager{};
    shaderManager shaderManager{};

    RenderTexture2D renderTarget;

    Game game{};
    Scene mainscene{"mainScene"};
    std::unique_ptr<NotificationManager> notificationManager = std::unique_ptr<NotificationManager>(new NotificationManager());
    std::unique_ptr<InputManager> inputManager = std::unique_ptr<InputManager>(new InputManager());

};

window::window() {}

window::~window() {}


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
    imguiRenderer.Init();

    renderTarget = meshrenderer.LoadRenderTextureWithDepth(width, height);
    meshrenderer.resultTexture = &renderTarget;

    imguiRenderer.game = &game;
    imguiRenderer.meshRenderer = &meshrenderer;
    imguiRenderer.screenTexture = &renderTarget;
    inputManager->scene = &game.scenes.at(0);

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    SetTargetFPS(60);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    SetConfigFlags(FLAG_WINDOW_HIGHDPI);

    rlSetClipPlanes(5.0f, 60.0f);

    float shadowMapResolution = 512 * 2;

    Vector3 viewPos = {0, 0, 0};
    Vector3 viewDir = {0, 0, 0};
    float rotation = 90;
    float uniformScale = 1.25f;
    float time = 0;

    meshManager.Import("./Assets/models/CafeBarRoom.glb");
    meshManager.Import("./Assets/models/CafeBar.glb");
    meshManager.Import("./Assets/models/CafeBar_Event_Ball.glb");
    meshManager.Import("./Assets/models/KitFox.glb");
    meshManager.Import("./Assets/models/Customers.glb");

    game.scenes.at(0).LoadScene(&meshManager);

    //Depth
    shaderManager.Import("./Assets/shader/shadowmapVShader.glsl;./Assets/shader/shadowmapFShader.glsl");
    game.layer[game.lastaddedLayer] = (renderer::RenderLayer{
        .name ="Depth",
        .clearBackground=true,
        .cullMode=RL_CULL_FACE_FRONT,
        .backgroundColor={255,255,255,1},
        .shader=shaderManager.list.back(),
    });
    //Shadow
    shaderManager.Import("./Assets/shader/vertexShader.glsl;./Assets/shader/shadowfragmentShader.glsl");
    game.layer[++game.lastaddedLayer] = (renderer::RenderLayer{
        .name ="Shadow",
        .clearBackground=true,
        .cullMode=RL_CULL_FACE_FRONT,
        .blendMode=RL_BLEND_MULTIPLIED,
        .backgroundColor={255,255,255,1},
        .shader=shaderManager.list.back(),
    });
    //pbr
    shaderManager.Import("./Assets/shader/vertexShader.glsl;./Assets/shader/fragmentShader.glsl");
    game.layer[++game.lastaddedLayer] = (renderer::RenderLayer{
        .name ="Pbr",
        .clearBackground=true,
        .backgroundColor={0,0,0,1},
        .shader=shaderManager.list.back(),
    });
    //screen
    shaderManager.Import(";./Assets/shader/screenFragmentShader.glsl");
    game.layer[++game.lastaddedLayer] = (renderer::RenderLayer{
        .name ="Screen",
        .backgroundColor={0,0,0,1},
        .shader=shaderManager.list.back(),
    });
    game.layer[++game.lastaddedLayer] = (renderer::RenderLayer{
        .name ="UI",
        .backgroundColor={0,0,0,1},
        .shader=shaderManager.list.back(),
    });

    meshrenderer.colorMap = LoadTexture("./Assets/texture/colorMap.png");

    SetTraceLogLevel(LOG_WARNING);
   
    GuiSetFont(config::getInstance().fontNormal);

    static constexpr int DefaultFonSize = 11;
    ImFontConfig defaultConfig;
    defaultConfig.SizePixels = DefaultFonSize;

    if (!IsWindowState(FLAG_WINDOW_HIGHDPI))
    {
        defaultConfig.SizePixels = defaultConfig.SizePixels * GetWindowScaleDPI().y;
        defaultConfig.RasterizerMultiply = GetWindowScaleDPI().y;
    }

    defaultConfig.PixelSnapH = true;
}

void window::Loop() {
    Ray ray = {0};
    RayCollision collision = {0};

    BeginDrawing();
        ClearBackground(BLACK);
        meshrenderer.Loop();
        inputManager->Update();

        rlEnableColorBlend();
        rlSetBlendMode(RL_BLEND_ALPHA);
        UIrendererer.Loop();
        imguiRenderer.Loop();
        //rlDisableColorBlend();
    EndDrawing();

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
    notificationManager->Update();
    inputManager->Update();
}

void window::Close() {

}
