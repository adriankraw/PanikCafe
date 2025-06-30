#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <cstdio>
#include <filesystem>
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include <cstddef>
#include <functional>
#include <iomanip>
#include <iostream>
#include <ostream>
#include <queue>
#include <string>

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
    if (IsKeyPressed(KEY_SPACE)) {
        printf("(%f/%f/%f)\n", lightPosition->x, lightPosition->y, lightPosition->z);
    }
}

struct light {
    Vector3 position;
    Vector3 direction;
    Vector3 Color{23.47f, 21.31f, 20.79f};
};

class GameObject {
  public:
    GameObject(){};
    GameObject(GameObject &&) = default;
    GameObject(const GameObject &) = default;
    GameObject &operator=(GameObject &&) = default;
    GameObject &operator=(const GameObject &) = default;
    ~GameObject() {};

    std::string name;
    Model model;
    Vector3 translations;
    Vector3 rotation;
    Vector3 scaling;
};
struct Game {
    std::string playerName;
    int playerAge;
    float playerActionValue;
    bool playerActionStarted;
    std::queue<std::function<void()>> playerAction;

    std::vector<GameObject> models;

    int customers;
    int customersMax;
    float customersCooldown;
    float customersCooldownMax;
    int smallCupsAmount;
    int bigCupsAmount;
    int beansAmount;
    int grindedBeansAmount;
    int cowMilkAmount;
    int oatMilkAmount;
    int madeEspressoAmount;
    int madeLatteAmount;
    int madeLongCafeAmount;
};

RenderTexture2D LoadRenderTextureWithDepth(int width, int height) {
    RenderTexture2D target = {0};

    target.id = rlLoadFramebuffer(); // Load an empty framebuffer

    if (target.id > 0) {
        rlEnableFramebuffer(target.id);

        // Create color texture (default to RGBA)
        target.texture.id = rlLoadTexture(0, width, height, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, 1);
        target.texture.width = width;
        target.texture.height = height;
        target.texture.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
        target.texture.mipmaps = 1;

        // Create depth texture
        target.depth.id = rlLoadTextureDepth(width, height, false);
        target.depth.width = width;
        target.depth.height = height;
        target.depth.format = PIXELFORMAT_UNCOMPRESSED_R32;
        target.depth.mipmaps = 1;

        // Attach color texture and depth texture to FBO
        rlFramebufferAttach(target.id, target.texture.id, RL_ATTACHMENT_COLOR_CHANNEL0, RL_ATTACHMENT_TEXTURE2D, 0);
        rlFramebufferAttach(target.id, target.depth.id, RL_ATTACHMENT_DEPTH, RL_ATTACHMENT_TEXTURE2D, 0);

        // Check if fbo is complete with attachments (valid)
        if (rlFramebufferComplete(target.id))
            TRACELOG(LOG_INFO, "FBO: [ID %i] Framebuffer object created successfully", target.id);

        rlDisableFramebuffer();
    } else
        TRACELOG(LOG_WARNING, "FBO: Framebuffer object can not be created");

    return target;
}

void increaseBeanCount(Game *_game) { _game->beansAmount++; }
void grindBeans(Game *_game) {
    _game->grindedBeansAmount++;
    _game->beansAmount--;
}
void increaseSmallCups(Game *_game) { _game->smallCupsAmount++; }
void increaseBigCups(Game *_game) { _game->bigCupsAmount++; }
void increaseCowMilkCount(Game *_game) { _game->cowMilkAmount++; }
void increaseOatMilkCount(Game *_game) { _game->oatMilkAmount++; }
void increaseEspressoAmount(Game *_game) { _game->madeEspressoAmount++; }
void increaseLatteAmount(Game *_game) { _game->madeLatteAmount++; }
void increaseLongCafeAmount(Game *_game) { _game->madeLongCafeAmount++; }

int main(int argc, char *argv[]) {

    InitWindow(1280, 720, "CodingProject");

    Camera3D mainCam = {0};
    mainCam.position = Vector3{10, 10, 10};
    mainCam.target = Vector3{0, 0, 0};
    mainCam.up = Vector3{0, 1, 0};
    mainCam.fovy = 45.0f;
    mainCam.projection = CAMERA_PERSPECTIVE;

    rlDisableBackfaceCulling();
    // rlDisableDepthMask();
    rlEnableDepthTest();
    SetTargetFPS(60);

    Vector3 objPosition = {0, 0, 0};
    Vector3 lightPosition = {7.0f, 4.0f, 6.0f};
    float shadowMapResolution = 512;
    Vector3 viewPos = {0, 0, 0};
    Vector3 viewDir = {0, 0, 0};
    float rotation = 90;
    float uniformScale = 1.25f;
    float time = 0;

    Game game = Game();
    game.playerName = "olantern";
    game.playerAge = 28;
    game.playerActionValue = 0.0f;
    game.playerActionStarted = false;
    game.customers = 0;
    game.customersMax = 10;
    game.customersCooldown = 60;
    game.customersCooldownMax = 60;
    game.smallCupsAmount = 10;
    game.bigCupsAmount = 10;
    game.beansAmount = 10;
    game.grindedBeansAmount = 0;
    game.cowMilkAmount = 0;
    game.oatMilkAmount = 0;
    game.madeEspressoAmount = 0;
    game.madeLatteAmount = 0;
    game.madeLongCafeAmount = 0;

    Model modelTarget = LoadModel("./CafeBar.glb");
    for (int i = 0; i < modelTarget.meshCount; ++i) {
        GameObject *gameO = new GameObject();
        gameO->name = "";
        gameO->model = LoadModelFromMesh(modelTarget.meshes[i]);
        gameO->translations = Vector3{0.0f, 0.0f, 0.0f};
        gameO->rotation = Vector3{0.0f, 0.0f, 0.0f};
        gameO->scaling = Vector3{uniformScale, uniformScale, uniformScale};

        game.models.emplace_back(*gameO);
    }

    int animCount = 0;
    unsigned int animIndex = 0;
    float animCurrentFrame = 0;
    ModelAnimation *modelAnimations = LoadModelAnimations("./CafeBar.glb", &animCount);
    std::printf(">> %i animations found \n", animCount);
    for (int i = 0; i < animCount; ++i) {
        auto animations = modelAnimations[i];
        std::printf("animation %i loaded\n", i);
        for (int f = 0; f < animations.frameCount; ++f) {
            Vector3 pose = animations.framePoses[f]->translation;
            std::printf("(%f %f %f)\n", pose.x, pose.y, pose.z);
        }
    }

    RenderTexture screenTexture = LoadRenderTexture(1280, 720);
    Shader screenShader = LoadShader(0, TextFormat("./screenFragmentShader.glsl", 330));
    Shader shadowmapShader =
        LoadShader(TextFormat("./shadowmapVShader.glsl", 330), TextFormat("./shadowmapFShader.glsl", 330));
    Camera3D lightCam = Camera3D{0};
    lightCam.position = Vector3Scale(lightPosition, 1.0f);
    lightCam.target = Vector3{0, 0, 0};
    lightCam.projection = CAMERA_ORTHOGRAPHIC;
    lightCam.up = Vector3{0, 1, 0};
    lightCam.fovy = 25.0f;
    RenderTexture2D shadowTexture = LoadRenderTextureWithDepth(shadowMapResolution, shadowMapResolution);
    SetTextureWrap(shadowTexture.texture, TEXTURE_WRAP_CLAMP);
    SetTextureFilter(shadowTexture.texture, RL_TEXTURE_FILTER_LINEAR);

    Shader coolShader = LoadShader(TextFormat("./vertexShader.glsl", 330), TextFormat("./fragmentShader.glsl", 330));

    Texture2D colorMap = LoadTexture("./texture/colorMap.png");

    SetTraceLogLevel(LOG_WARNING);
    std::printf(">> loading GameStruct\n");
    // Load GameStruct

    while (!WindowShouldClose()) {
        game.customersCooldown -= GetFrameTime();
        ReadInput(&lightPosition, coolShader);
        lightCam.position = Vector3Scale(lightPosition, 1.0f);

        for (int anim_index = 0; anim_index < animCount; ++anim_index) {
            ModelAnimation anim = modelAnimations[animIndex];
            animCurrentFrame = (animCurrentFrame + 1.0f);
            if (animCurrentFrame >= anim.frameCount)
                animCurrentFrame = 0;
        }

        // shadowRender
        Matrix lightProj;
        Matrix lightView;
        for (size_t gO = 0; gO < game.models.size(); ++gO) {
            for (size_t ma = 0; ma < modelTarget.materialCount; ++ma) {
                game.models[gO].model.materials[ma].shader = shadowmapShader;
            }
        }

        rlSetCullFace(rlCullMode::RL_CULL_FACE_BACK);
        rlSetClipPlanes(0.01f, 100.0f);
        std::printf(">>start \n");
        BeginTextureMode(shadowTexture);
        {
            ClearBackground(RAYWHITE);
            BeginMode3D(lightCam);
            {
                lightView = rlGetMatrixModelview();
                lightProj = rlGetMatrixProjection();
                for (auto _model: game.models) {
                    std::printf(">>.");
                    DrawModel(_model.model, _model.translations, 1.0f, WHITE);
                }
            }
            EndMode3D();
        }
        EndTextureMode();
        std::printf(">> test\n");
        Matrix lightViewProj = MatrixMultiply(lightView, lightProj);
        SetShaderValueMatrix(coolShader, rlGetLocationUniform(coolShader.id, "lightViewProj"), lightViewProj);

        // light & color
        viewPos = mainCam.position;
        viewDir = Vector3Normalize(mainCam.target - viewPos);
        SetShaderValueV(coolShader, rlGetLocationUniform(coolShader.id, "lightPos"), &lightPosition,
                        SHADER_UNIFORM_VEC3, 1);
        SetShaderValueV(coolShader, rlGetLocationUniform(coolShader.id, "viewPos"), &viewPos, SHADER_UNIFORM_VEC3, 1);
        SetShaderValueV(coolShader, rlGetLocationUniform(coolShader.id, "viewDir"), &viewDir, SHADER_UNIFORM_VEC3, 1);
        // SetShaderValueTexture(coolShader, GetShaderLocation(coolShader, "shadow0"), shadowTexture.depth);

        for (size_t ma = 0; ma < modelTarget.materialCount; ++ma) {
            modelTarget.materials[ma].shader = coolShader;
        }
        SetShaderValue(coolShader, GetShaderLocation(coolShader, "shadowMapResolution"), &shadowMapResolution,
                       SHADER_UNIFORM_INT);
        rlSetCullFace(rlCullMode::RL_CULL_FACE_FRONT);
        BeginTextureMode(screenTexture);
        {
            ClearBackground(RAYWHITE);

            BeginMode3D(mainCam);
            {
                for (int mesh_index = 0; mesh_index < modelTarget.meshCount; ++mesh_index) {
                    Texture roughnessMap = modelTarget.materials[modelTarget.meshMaterial[mesh_index]]
                                               .maps[MATERIAL_MAP_ROUGHNESS]
                                               .texture;
                    Texture metalicMap = modelTarget.materials[modelTarget.meshMaterial[mesh_index]]
                                             .maps[MATERIAL_MAP_METALNESS]
                                             .texture;

                    if (roughnessMap.id) {
                        SetShaderValueTexture(coolShader, GetShaderLocation(coolShader, "roughness0"), roughnessMap);
                    } else {
                        const float inserted = 0.8f;
                        SetShaderValue(coolShader, GetShaderLocation(coolShader, "roughness1"), &inserted,
                                       RL_SHADER_UNIFORM_FLOAT);
                    }
                    if (metalicMap.id) {
                        SetShaderValueTexture(coolShader, GetShaderLocation(coolShader, "metalic0"), metalicMap);
                    } else {
                        const float inserted = 0.8f;
                        SetShaderValue(coolShader, GetShaderLocation(coolShader, "metalic1"), &inserted,
                                       RL_SHADER_UNIFORM_FLOAT);
                    }
                    if (shadowTexture.depth.id)
                        SetShaderValueTexture(coolShader, GetShaderLocation(coolShader, "shadow0"),
                                              shadowTexture.depth);

                    for (int model_index = 0; model_index < game.models.size(); ++model_index) {
                        DrawModel(game.models[model_index].model, game.models[model_index].translations, 1.0f, WHITE);
                    }
                }
                // DrawModel(sphere, objPosition, 1.0f, RAYWHITE);
            }
            // Draw lightSphere
            DrawSphere(lightPosition, 0.2f, BLUE);
            EndMode3D();
        }
        EndTextureMode();

        // screentexture
        BeginDrawing();
        {
            ClearBackground(RAYWHITE);

            BeginShaderMode(screenShader);
            {
                SetShaderValueTexture(screenShader, rlGetLocationUniform(screenShader.id, "colormap"), colorMap);
                DrawTextureRec(
                    screenTexture.texture,
                    Rectangle{0, 0, (float)screenTexture.texture.width, (float)-screenTexture.texture.height},
                    Vector2{0, 0}, WHITE);
                // DrawTexture(shadowTexture.texture, 0, 0, wHITE);
            }
            EndShaderMode();

            DrawRectangle(screenTexture.texture.width - 50, (screenTexture.texture.height - 200.0f) / 2, 30, 200, BLUE);
            DrawRectangle(screenTexture.texture.width - 50 + 4, (screenTexture.texture.height - 200.0f) / 2 + 4, 30 - 8,
                          200 - 8, DARKBLUE);
            std::time_t currentClock = std::time(nullptr);
            char timebuffer[6];
            std::strftime(timebuffer, sizeof(timebuffer), "%H:%M", std::localtime(&currentClock));

            Rectangle buttonRect{30, 100, 100, 25};
            Rectangle labelRect{150, 100, 100, 25};
            if (GuiButton(buttonRect, "get Beans")) {
                if (game.playerActionStarted == false) {
                    game.playerAction.push([&game]() { increaseBeanCount(&game); });
                    game.playerActionStarted = true;
                }
            }
            GuiLabel(labelRect, std::string("Beans: ").append(std::to_string(game.beansAmount)).c_str());
            buttonRect.y += 30;
            labelRect.y += 30;
            if (GuiButton(buttonRect, "grind beans")) {
                if (game.playerActionStarted == false) {
                    game.playerAction.push([&game]() { grindBeans(&game); });
                    game.playerActionStarted = true;
                    printf("grind beans \n");
                }
            }
            GuiLabel(labelRect, std::string("Grind. beans: ").append(std::to_string(game.grindedBeansAmount)).c_str());
            buttonRect.y += 50;
            labelRect.y += 50;
            if (GuiButton(buttonRect, "Small Cups")) {
                if (game.playerActionStarted == false) {
                    game.playerAction.push([&game]() { increaseSmallCups(&game); });
                    game.playerActionStarted = true;
                    printf("get small cups \n");
                }
            }
            GuiLabel(labelRect, std::string("Small Cups: ").append(std::to_string(game.smallCupsAmount)).c_str());
            buttonRect.y += 30;
            labelRect.y += 30;
            if (GuiButton(buttonRect, "Large Cups")) {
                if (game.playerActionStarted == false) {
                    game.playerAction.push([&game]() { increaseBigCups(&game); });
                    game.playerActionStarted = true;
                    printf("Large Cups \n");
                }
            }
            GuiLabel(labelRect, std::string("Large Cups: ").append(std::to_string(game.bigCupsAmount)).c_str());
            buttonRect.y += 30;
            labelRect.y += 30;
            if (GuiButton(buttonRect, "cow Milk")) {
                if (game.playerActionStarted == false) {
                    game.playerAction.push([&game]() { increaseCowMilkCount(&game); });
                    game.playerActionStarted = true;
                    printf("get cow Milk \n");
                }
            }
            GuiLabel(labelRect, std::string("Cow Milk: ").append(std::to_string(game.cowMilkAmount)).c_str());
            buttonRect.y += 30;
            labelRect.y += 30;
            if (GuiButton(buttonRect, "oat Milk")) {
                if (game.playerActionStarted == false) {
                    game.playerAction.push([&game]() { increaseOatMilkCount(&game); });
                    game.playerActionStarted = true;
                    printf("get oat Milk \n");
                }
            }
            GuiLabel(labelRect, std::string("Oat Milk: ").append(std::to_string(game.oatMilkAmount)).c_str());
            buttonRect.y += 50;
            labelRect.y += 50;
            if (GuiButton(buttonRect, "Espresso")) {
                if (game.playerActionStarted == false) {
                    game.playerAction.push([&game]() { increaseEspressoAmount(&game); });
                    game.playerActionStarted = true;
                    printf("do Espresso \n");
                }
            }
            GuiLabel(labelRect, std::string("Espresso: ").append(std::to_string(game.madeEspressoAmount)).c_str());
            buttonRect.y += 30;
            labelRect.y += 30;
            if (GuiButton(buttonRect, "Latte")) {
                if (game.playerActionStarted == false) {
                    game.playerAction.push([&game]() { increaseLatteAmount(&game); });
                    game.playerActionStarted = true;
                    printf("do Latte \n");
                }
            }
            GuiLabel(labelRect, std::string("Late: ").append(std::to_string(game.madeLatteAmount)).c_str());
            buttonRect.y += 30;
            labelRect.y += 30;
            if (GuiButton(buttonRect, "LongCafe")) {
                if (game.playerActionStarted == false) {
                    game.playerAction.push([&game]() { increaseLongCafeAmount(&game); });
                    game.playerActionStarted = true;
                    printf("do LongCafe \n");
                }
            }
            GuiLabel(labelRect, std::string("Long Cafe: ").append(std::to_string(game.madeLongCafeAmount)).c_str());

            if (game.playerActionStarted) {
                game.playerActionValue += GetFrameTime() / 10.0f;
                float actionBarWidth = 500.0f;
                float actionBarHeight = 50.0f;
                GuiProgressBar(Rectangle{(screenTexture.texture.width - actionBarWidth) / 2,
                                         screenTexture.texture.height - 50.0f, actionBarWidth, actionBarHeight},
                               "", "", &game.playerActionValue, 0, 1);
                if (game.playerActionValue == 1) {
                    if (!game.playerAction.empty()) {
                        std::function<void()> action = game.playerAction.front();
                        action();
                        game.playerAction.pop();
                        game.playerActionValue = 0;
                        if (game.playerAction.empty()) {
                            game.playerActionStarted = false;
                        }
                    }
                }
            }
            DrawText(timebuffer, 5, 5, 48, BLACK);
        }
        EndDrawing();
    }
    rlUnloadFramebuffer(shadowTexture.id);
    UnloadShader(coolShader);
    UnloadShader(shadowmapShader);
    UnloadModel(modelTarget);
    return 0;
}
