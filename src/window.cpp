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

#include <ctime>
#include <iostream>
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

    bool inputActive = false;

  private:
    meshRenderer Meshrenderer{};
    uiRenderer UIrendererer{};
    imguiRenderer ImguiRenderer{};

    meshManager MeshManager{};
    shaderManager ShaderManager{};

    RenderTexture2D renderTarget;

    Game game{};
    Scene mainscene{"mainScene"};
    std::unique_ptr<NotificationManager> notificationManager = std::unique_ptr<NotificationManager>(new NotificationManager());
    std::unique_ptr<InputManager> inputManager = std::unique_ptr<InputManager>(new InputManager());

};

window::window() {}

window::~window() {}


void window::Init(int width, int height, const std::string title) {


    SetConfigFlags(FLAG_MSAA_4X_HINT);
    SetTargetFPS(30);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    SetConfigFlags(FLAG_WINDOW_HIGHDPI);

    InitWindow(width, height, title.c_str());
    config::getInstance().LoadConfig();
    
    float clipmin = 1.0f;
    float clipmax = 60.0f;
    mainscene.clipRange = Vector2{clipmin, clipmax};
    game.scenes.push_back(mainscene);

    GetFontDefault() = config::getInstance().fontNormal;

    Meshrenderer.game = &game;
    Meshrenderer.scene = &game.scenes.at(0);
    UIrendererer.game = &game;
    Meshrenderer.Init();
    UIrendererer.Init();
    ImguiRenderer.Init();

    renderTarget = Meshrenderer.LoadRenderTextureWithDepth(width, height);
    Meshrenderer.resultTexture = &renderTarget;

    ImguiRenderer.game = &game;
    ImguiRenderer.screenTexture = &renderTarget;
    inputManager->scene = &game.scenes.at(0);

    rlSetClipPlanes(clipmin, clipmax);

    float shadowMapResolution = 512 * 2;

    Vector3 viewPos = {0, 0, 0};
    Vector3 viewDir = {0, 0, 0};
    float rotation = 90;
    float uniformScale = 1.25f;
    float time = 0;

    FilePathList files = LoadDirectoryFilesEx("./Assets/models", ".glb", false);
    for (size_t fileIndex = 0; fileIndex < files.count; ++fileIndex)
    {
        MeshManager.Import(files.paths[fileIndex]);
    }
    game.scenes.at(0).LoadScene(&MeshManager);

    game.layer[0] = (renderer::RenderLayer{
        .name = "null",
        .nextNode = 1,
        .nodeType = renderer::RenderState::null
    });
    //Depth
    ShaderManager.Import("./Assets/shader/shadowmapVShader.glsl;./Assets/shader/shadowmapFShader.glsl");
    //Shadow
    ShaderManager.Import("./Assets/shader/vertexShader.glsl;./Assets/shader/shadowfragmentShader.glsl");
    //pbr
    ShaderManager.Import("./Assets/shader/vertexShader.glsl;./Assets/shader/fragmentShader.glsl");
    //screen
    ShaderManager.Import(";./Assets/shader/screenFragmentShader.glsl");

    Meshrenderer.colorMap = LoadTexture("./Assets/texture/colorMap.png");

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
    inputActive = ImguiRenderer.ishovered;

    Ray ray = {0};
    RayCollision collision = {0};

    for(int i = 0; i < game.lastaddedLayer; ++i){
        if(game.layer[i].shaderVertexModTime != GetFileModTime(game.layer[i].shaderVertex.c_str())){
            game.layer[i].shaderVertexModTime = GetFileModTime(game.layer[i].shaderVertex.c_str());
            game.layer[i].reloadShader = true;
        }
        if(game.layer[i].shaderFragmentModTime != GetFileModTime(game.layer[i].shaderFragment.c_str())){
            game.layer[i].shaderFragmentModTime = GetFileModTime(game.layer[i].shaderFragment.c_str());
            game.layer[i].reloadShader = true;
        }
        if(game.layer[i].reloadShader){
            int vl = game.layer[i].shaderVertex.length();
            game.layer[i].shader = LoadShader(
                        vl>0?game.layer[i].shaderVertex.c_str():0,
                        game.layer[i].shaderFragment.c_str()
                    );
            game.layer[i].reloadShader = false;
        }
    }


    BeginDrawing();
        ClearBackground(BLACK);
        Meshrenderer.Loop();

        if(inputActive){
            inputManager->Update();
        }
        //rlClearScreenBuffers();
        BeginTextureMode(*Meshrenderer.resultTexture);
        rlSetBlendMode(BlendMode::BLEND_ALPHA);
        UIrendererer.Loop();
        EndTextureMode();

        ImguiRenderer.Loop();
    EndDrawing();
    

    /*
    Light *lightCam = &game.scenes[0].lights[0];
    ReadInput(&lightCam->cam.position, Meshrenderer.shader[renderer::RenderState::shadow]); // light movement WASD

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
}

void window::Close() {

}
