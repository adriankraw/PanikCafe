#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <cstdio>
#include <filesystem>
#include <list>
#include <memory>
#include <vector>
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include <cstddef>
#include <functional>
#include <iomanip>
#include <iostream>
#include <ostream>
#include <queue>
#include <string>

#include "customer.cpp"
#include "game.cpp"
#include "gameObject.cpp"
#include "notif/notificationManager.cpp"

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
        manager->AddNotification(NotificationEnum::ALERT, "alleeeerrrrtttt!");
        //printf("(%f/%f/%f)\n", lightPosition->x, lightPosition->y, lightPosition->z);
    }
}

struct light {
    Vector3 position;
    Vector3 direction;
    Vector3 Color{23.47f, 21.31f, 20.79f};
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

Model* ImportModelWithAnimations(Game *game, std::string path) {
    Model modelTarget = LoadModel(path.c_str());
    game->models.emplace_back(GameObject("", modelTarget, Vector3{0, 0, 0}, Vector3{0, 0, 0}, Vector3{1, 1, 1}));

    int animCount = 0;
    unsigned int animIndex = 0;
    ModelAnimation *modelAnimations = LoadModelAnimations(path.c_str(), &animCount);
    std::printf(">> %i animations found \n", animCount);
    for (int i = 0; i < animCount; ++i) {
        auto animations = modelAnimations[i];

        if(animations.frameCount<=1) continue; // dont care about animations containing only 1 frame ...

        game->models.back().animations.emplace_back(animations);
        std::printf("animation %i loaded\n", i);
        for (int f = 0; f < animations.frameCount; ++f) {
            // Vector3 pose = animations.framePoses[f]->translation;
            Vector3 pose = modelTarget.bindPose[f].translation;
            std::printf("(%f %f %f)\n", pose.x, pose.y, pose.z);
        }
    }
    return &(game->models.back().model);
}

int main(int argc, char *argv[]) {

    InitWindow(1280, 720, "CodingProject");

    config::getInstance().LoadConfig();
    GetFontDefault() = config::getInstance().fontNormal;

    // default materialValue
    float def_roughness = 0.4f;
    float def_metalic = 0.1f;

    Camera3D mainCam = {0};
    mainCam.position = Vector3{10, 10, 10};
    mainCam.target = Vector3{0, 0, 0};
    mainCam.up = Vector3{0, 1, 0};
    mainCam.fovy = 45.0f;
    mainCam.projection = CAMERA_PERSPECTIVE;

    //rlDisableBackfaceCulling();
    //rlEnableBackfaceCulling();
    //rlEnableDepthTest();
    //rlEnableDepthMask();
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    SetTargetFPS(60);

    Vector3 objPosition = {0, 0, 0};
    Vector3 lightPosition = Vector3{0.0f, 6.0f, 0.0f};
    float shadowMapResolution = 512*2;
    Vector3 viewPos = {0, 0, 0};
    Vector3 viewDir = {0, 0, 0};
    float rotation = 90;
    float uniformScale = 1.25f;
    float time = 0;

    Game game = Game();

    Material defaultMat = LoadMaterialDefault();
    ImportModelWithAnimations(&game, "./CafeBarRoom.glb");
    game.models.back().depthIgnore = true;
    ImportModelWithAnimations(&game, "./CafeBar.glb");
    ImportModelWithAnimations(&game, "./CafeBar_Event_Ball.glb");
    game.playerModel = ImportModelWithAnimations(&game, "./KitFox.glb");


    Model customersSpawnPoints = LoadModel("./Customers.glb");
    for (int c = 0; c < game.customers.size(); ++c) {
        game.customers[c].mesh = &(customersSpawnPoints.meshes[c]);
    }

    RenderTexture screenTexture = LoadRenderTexture(1280, 720);
    Shader screenShader = LoadShader(0, TextFormat("./screenFragmentShader.glsl", 330));
    Shader shadowmapShader =
        LoadShader(TextFormat("./shadowmapVShader.glsl", 330), TextFormat("./shadowmapFShader.glsl", 330));
    Camera3D lightCam = Camera3D{0};
    lightCam.position = lightPosition;
    lightCam.target = mainCam.target;
    lightCam.projection = CAMERA_ORTHOGRAPHIC;
    lightCam.up = Vector3{0, 1, 0};
    lightCam.fovy = 20.0f;
    RenderTexture2D shadowTexture = LoadRenderTextureWithDepth(shadowMapResolution, shadowMapResolution);
    SetTextureWrap(shadowTexture.texture, TEXTURE_WRAP_CLAMP);
    SetTextureFilter(shadowTexture.texture, RL_TEXTURE_FILTER_LINEAR);

    Shader coolShader = LoadShader(TextFormat("./vertexShader.glsl", 330), TextFormat("./fragmentShader.glsl", 330));

    Texture2D colorMap = LoadTexture("./texture/colorMap.png");

    SetTraceLogLevel(LOG_WARNING);

    Ray ray = {0};
    RayCollision collision = {0};

    while (!WindowShouldClose()) {
        ReadInput(&lightPosition, coolShader); // light movement WASD
        lightCam.position = lightPosition;
        ReadInput(mainCam);                    // camera movement

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
                        if(cust.need_sup != CustomerNeed_Sup::Sup_None){
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

        // shadowRender
        SetShaderValueV(shadowmapShader, rlGetLocationUniform(shadowmapShader.id, "lightPos"), &lightPosition,
                        SHADER_UNIFORM_VEC3, 1);
        Matrix lightProj;
        Matrix lightView;
        defaultMat.shader = shadowmapShader;
        for (size_t gO = 0; gO < game.models.size(); ++gO) {
            for (int anim_index = 0; anim_index < game.models[gO].animations.size(); ++anim_index) {
                game.models[gO].animationsFrame += 1;
                if (game.models[gO].animationsFrame >= game.models[gO].animations[anim_index].frameCount)
                    game.models[gO].animationsFrame = 0;
            }
            for (size_t ma = 0; ma < game.models[gO].model.materialCount; ++ma) {
                game.models[gO].model.materials[ma].shader = shadowmapShader;
            }
        }
        rlDisableBackfaceCulling();
        rlSetCullFace(rlCullMode::RL_CULL_FACE_FRONT);
        BeginTextureMode(shadowTexture);
        {
            ClearBackground(RAYWHITE);
            BeginMode3D(lightCam);
            {
                lightView = rlGetMatrixModelview();
                lightProj = rlGetMatrixProjection();
                for (int model_index = 0; model_index < game.models.size(); ++model_index) {
                    //if (game.models[model_index].depthIgnore) continue;

                    Vector3 trans = game.models[model_index].translation;
                    if (game.models[model_index].animations.size() > 0) {
                        float frame = game.models[model_index].animationsFrame;
                        trans.x -= game.models[model_index]
                                       .animations[game.models[model_index].animationCurrent]
                                       .framePoses[0]
                                       ->translation.x /
                                   1;
                        trans.y -= game.models[model_index]
                                       .animations[game.models[model_index].animationCurrent]
                                       .framePoses[0]
                                       ->translation.z /
                                   1;
                        trans.z -= game.models[model_index]
                                       .animations[game.models[model_index].animationCurrent]
                                       .framePoses[0]
                                       ->translation.y /
                                   -1;

                        trans.x += game.models[model_index]
                                       .animations[game.models[model_index].animationCurrent]
                                       .framePoses[(int)frame]
                                       ->translation.x /
                                   1;
                        trans.y += game.models[model_index]
                                       .animations[game.models[model_index].animationCurrent]
                                       .framePoses[(int)frame]
                                       ->translation.z /
                                   1;
                        trans.z += game.models[model_index]
                                       .animations[game.models[model_index].animationCurrent]
                                       .framePoses[(int)frame]
                                       ->translation.y /
                                   -1;
                    }
                    DrawModelEx(game.models[model_index].model, trans, Vector3{0, 0, 0}, 1.0f, Vector3{1, 1, 1}, WHITE);
                }
                for (size_t cu = 0; cu < game.customers.size(); ++cu) {
                    if (game.customers[cu].active) {
                        float clicked = game.customers[cu].clicked ? 1 : 0;
                        DrawMesh(*game.customers[cu].mesh, defaultMat, customersSpawnPoints.transform);
                    }
                }
            }
            EndMode3D();
        }
        EndTextureMode();
        Matrix lightViewProj = MatrixMultiply(lightView, lightProj);
        SetShaderValueMatrix(coolShader, rlGetLocationUniform(coolShader.id, "lightViewProj"), lightViewProj);

        // light & color
        viewPos = mainCam.position;
        viewDir = Vector3Normalize(mainCam.target - viewPos);
        SetShaderValueV(coolShader, rlGetLocationUniform(coolShader.id, "lightPos"), &lightPosition,
                        SHADER_UNIFORM_VEC3, 1);
        SetShaderValueV(coolShader, rlGetLocationUniform(coolShader.id, "viewPos"), &viewPos, SHADER_UNIFORM_VEC3, 1);
        SetShaderValueV(coolShader, rlGetLocationUniform(coolShader.id, "viewDir"), &viewDir, SHADER_UNIFORM_VEC3, 1);
        SetShaderValueTexture(coolShader, GetShaderLocation(coolShader, "shadow0"), shadowTexture.depth);

        defaultMat.shader = coolShader;
        for (size_t gO = 0; gO < game.models.size(); ++gO) {
            for (size_t ma = 0; ma < game.models[gO].model.materialCount; ++ma) {
                game.models[gO].model.materials[ma].shader = coolShader;
            }
        }
        SetShaderValue(coolShader, GetShaderLocation(coolShader, "shadowMapResolution"), &shadowMapResolution,
                       SHADER_UNIFORM_INT);
        //rlEnableBackfaceCulling();
        rlSetCullFace(rlCullMode::RL_CULL_FACE_FRONT);
        BeginTextureMode(screenTexture);
        {
            ClearBackground(RAYWHITE);

            BeginMode3D(mainCam);
            {
                for (size_t gO = 0; gO < game.models.size(); ++gO) {
                    game.models[gO].model.materials->shader.locs[SHADER_LOC_MAP_ROUGHNESS] =
                        game.models[gO]
                            .model.materials[game.models[gO].model.meshMaterial[0]]
                            .maps[MATERIAL_MAP_ROUGHNESS]
                            .texture.id;
                    if (game.models[gO].model.materials->shader.locs[SHADER_LOC_MAP_ROUGHNESS] <= 0) {
                        SetShaderValue(coolShader, GetShaderLocation(coolShader, "roughness1"), &def_roughness,
                                       SHADER_UNIFORM_FLOAT);
                    }
                    game.models[gO].model.materials->shader.locs[SHADER_LOC_MAP_METALNESS] =
                        game.models[gO]
                            .model.materials[game.models[gO].model.meshMaterial[0]]
                            .maps[MATERIAL_MAP_METALNESS]
                            .texture.id;
                    if (game.models[gO].model.materials->shader.locs[SHADER_LOC_MAP_METALNESS] <= 0) {
                        SetShaderValue(coolShader, GetShaderLocation(coolShader, "metalic1"), &def_metalic,
                                       SHADER_UNIFORM_FLOAT);
                    }

                    if (shadowTexture.depth.id)
                        SetShaderValueTexture(coolShader, GetShaderLocation(coolShader, "shadow0"),
                                              shadowTexture.depth);

                    for (int model_index = 0; model_index < game.models.size(); ++model_index) {
                        Vector3 trans = game.models[model_index].translation;
                        if (game.models[model_index].animations.size() > 0) {
                            float frame = game.models[model_index].animationsFrame;
                            trans.x -= game.models[model_index]
                                           .animations[game.models[model_index].animationCurrent]
                                           .framePoses[0]
                                           ->translation.x /
                                       1;
                            trans.y -= game.models[model_index]
                                           .animations[game.models[model_index].animationCurrent]
                                           .framePoses[0]
                                           ->translation.z /
                                       1;
                            trans.z -= game.models[model_index]
                                           .animations[game.models[model_index].animationCurrent]
                                           .framePoses[0]
                                           ->translation.y /
                                       -1;

                            trans.x += game.models[model_index]
                                           .animations[game.models[model_index].animationCurrent]
                                           .framePoses[(int)frame]
                                           ->translation.x /
                                       1;
                            trans.y += game.models[model_index]
                                           .animations[game.models[model_index].animationCurrent]
                                           .framePoses[(int)frame]
                                           ->translation.z /
                                       1;
                            trans.z += game.models[model_index]
                                           .animations[game.models[model_index].animationCurrent]
                                           .framePoses[(int)frame]
                                           ->translation.y /
                                       -1;
                        }
                        DrawModelEx(game.models[model_index].model, trans, Vector3{0, 0, 0}, 1.0f, Vector3{1, 1, 1},
                                    WHITE);
                    }
                }
                float clicked = 0;
                for (size_t cu = 0; cu < game.customers.size(); ++cu) {
                    if (game.customers[cu].active) {
                        clicked = game.customers[cu].clicked ? 1 : 0;
                        SetShaderValue(coolShader, GetShaderLocation(coolShader, "selected"), &clicked,
                                       SHADER_UNIFORM_FLOAT);
                        DrawMesh(*game.customers[cu].mesh, defaultMat, customersSpawnPoints.transform);
                    }
                }
                clicked = 0;
                SetShaderValue(coolShader, GetShaderLocation(coolShader, "selected"), &clicked, SHADER_UNIFORM_FLOAT);
            }
            // Draw lightSphere
            DrawSphere(lightPosition, 0.2f, BLUE);
            EndMode3D();
        }
        EndTextureMode();
        
        //Raycast Mouseclick on customers
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            if (!collision.hit || true) {
                std::list<int> clickedLastFrame;
                int clickedThisFrame;
                ray = GetScreenToWorldRay(GetMousePosition(), mainCam);
                for (size_t cu = 0; cu < game.customers.size(); ++cu) {
                    if (game.customers[cu].active) {
                        if (game.customers[cu].clicked) {
                            clickedLastFrame.push_back(cu);
                        }
                        collision = GetRayCollisionMesh(ray, *game.customers[cu].mesh, customersSpawnPoints.transform);
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
                    game.playerActionDuration = 1.0f;
                    game.playerAction.push([&game]() { game.increaseBeanCount(); });
                    game.playerActionStarted = true;
                }
            }
            GuiLabel(labelRect, std::string("Beans: ").append(std::to_string(game.beansAmount)).c_str());
            buttonRect.y += 30;
            labelRect.y += 30;
            if (GuiButton(buttonRect, "grind beans")) {
                if (game.playerActionStarted == false) {
                    game.playerActionDuration = 1.0f;
                    game.playerAction.push([&game]() { game.grindBeans(); });
                    game.playerActionStarted = true;
                }
            }
            GuiLabel(labelRect, std::string("Grind. beans: ").append(std::to_string(game.grindedBeansAmount)).c_str());
            buttonRect.y += 50;
            labelRect.y += 50;
            if (GuiButton(buttonRect, "Small Cups")) {
                if (game.playerActionStarted == false) {
                    game.playerActionDuration = 2.0f;
                    game.playerAction.push([&game]() { game.increaseSmallCups(); });
                    game.playerActionStarted = true;
                }
            }
            GuiLabel(labelRect, std::string("Small Cups: ").append(std::to_string(game.smallCupsAmount)).c_str());
            buttonRect.y += 30;
            labelRect.y += 30;
            if (GuiButton(buttonRect, "Large Cups")) {
                if (game.playerActionStarted == false) {
                    game.playerActionDuration = 2.0f;
                    game.playerAction.push([&game]() { game.increaseBigCups(); });
                    game.playerActionStarted = true;
                }
            }
            GuiLabel(labelRect, std::string("Large Cups: ").append(std::to_string(game.bigCupsAmount)).c_str());
            buttonRect.y += 30;
            labelRect.y += 30;
            if (GuiButton(buttonRect, "cow Milk")) {
                if (game.playerActionStarted == false) {
                    game.playerActionDuration = 2.0f;
                    game.playerAction.push([&game]() { game.increaseCowMilkCount(); });
                    game.playerActionStarted = true;
                }
            }
            GuiLabel(labelRect, std::string("Cow Milk: ").append(std::to_string(game.cowMilkAmount)).c_str());
            buttonRect.y += 30;
            labelRect.y += 30;
            if (GuiButton(buttonRect, "oat Milk")) {
                if (game.playerActionStarted == false) {
                    game.playerActionDuration = 2.0f;
                    game.playerAction.push([&game]() { game.increaseOatMilkCount(); });
                    game.playerActionStarted = true;
                }
            }
            GuiLabel(labelRect, std::string("Oat Milk: ").append(std::to_string(game.oatMilkAmount)).c_str());
            buttonRect.y += 50;
            labelRect.y += 50;
            if (GuiButton(buttonRect, "Espresso")) {
                if (game.playerActionStarted == false) {
                    game.playerActionDuration = 32.0f;
                    game.playerAction.push([&game]() { game.increaseEspressoAmount(); });
                    game.playerActionStarted = true;
                }
            }
            GuiLabel(labelRect, std::string("Espresso: ").append(std::to_string(game.madeEspressoAmount)).c_str());
            buttonRect.y += 30;
            labelRect.y += 30;
            if (GuiButton(buttonRect, "Latte")) {
                if (game.playerActionStarted == false) {
                    game.playerActionDuration = 40.0f;
                    game.playerAction.push([&game]() { game.increaseLatteAmount(); });
                    game.playerActionStarted = true;
                }
            }
            GuiLabel(labelRect, std::string("Late: ").append(std::to_string(game.madeLatteAmount)).c_str());
            buttonRect.y += 30;
            labelRect.y += 30;
            if (GuiButton(buttonRect, "LongCafe")) {
                if (game.playerActionStarted == false) {
                    game.playerActionDuration = 50.0f;
                    game.playerAction.push([&game]() { game.increaseLongCafeAmount(); });
                    game.playerActionStarted = true;
                }
            }
            GuiLabel(labelRect, std::string("Long Cafe: ").append(std::to_string(game.madeLongCafeAmount)).c_str());
            buttonRect.y += 60;
            labelRect.y += 60;
            if (GuiButton(buttonRect, "serve")) {
                if (game.selectedCustomer != nullptr) {
                    game.serveCustomer(game.selectedCustomer);
                    if (game.checkCustomer(game.selectedCustomer)) {
                        game.selectedCustomer->served = true;
                        game.selectedCustomer->active = false;
                        game.selectedCustomer->clicked = false;
                        game.customersCount--;
                    }
                }
            }
            GuiLabel(labelRect, game.GetFreeCustomer() != nullptr ? game.GetFreeCustomer()->name.c_str(): "");

            if (game.playerActionStarted) {
                game.playerActionValue += GetFrameTime();
                float actionBarWidth = 500.0f;
                float actionBarHeight = 50.0f;
                float actionBarStartValue = 0;
                float actionBarEndValue = game.playerActionDuration;
                GuiProgressBar(Rectangle{(screenTexture.texture.width - actionBarWidth) / 2,
                                         screenTexture.texture.height - 50.0f, actionBarWidth, actionBarHeight},
                               "", std::to_string(game.playerActionDuration).c_str(), &game.playerActionValue, actionBarStartValue, actionBarEndValue);
                if (game.playerActionValue == actionBarEndValue) {
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
            DrawTexturePro(shadowTexture.depth, Rectangle{0,0,(float)shadowTexture.depth.width, -(float)shadowTexture.depth.height}, Rectangle{0,0,250,250}, Vector2{0,0}, 0, RAYWHITE);
            DrawText(timebuffer, 5, 5, 48, BLACK);
            DrawText(std::to_string((int)(1.0f/GetFrameTime())).c_str(), screenTexture.texture.width - 100, 5, 48, BLACK);
        }
        manager->Update();
        EndDrawing();
    }
    rlUnloadFramebuffer(shadowTexture.id);
    UnloadShader(coolShader);
    UnloadShader(shadowmapShader);
    UnloadModel(customersSpawnPoints);
    for (size_t gO = 0; gO < game.models.size(); ++gO) {
        UnloadModel(game.models[gO].model);
    }
    return 0;
}
