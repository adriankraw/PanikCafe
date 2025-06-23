#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <filesystem>
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include <cstddef>
#include <iostream>
#include <iomanip>
#include <ostream>
#include <string>

void ReadInput(Vector3 *lightPosition, Shader &coolShader) {
    if (IsKeyDown(KEY_A)) {
        lightPosition->x += -5 * GetFrameTime();
        SetShaderValue(coolShader, rlGetLocationUniform(coolShader.id, "lightPos"), lightPosition,
                       SHADER_UNIFORM_VEC3);
    }
    if (IsKeyDown(KEY_D)) {
        lightPosition->x += 5 * GetFrameTime();
        SetShaderValue(coolShader, rlGetLocationUniform(coolShader.id, "lightPos"), lightPosition,
                       SHADER_UNIFORM_VEC3);
    }
    if (IsKeyDown(KEY_W)) {
        lightPosition->z += 5 * GetFrameTime();
        SetShaderValue(coolShader, rlGetLocationUniform(coolShader.id, "lightPos"), lightPosition,
                       SHADER_UNIFORM_VEC3);
    }
    if (IsKeyDown(KEY_S)) {
        lightPosition->z += -5 * GetFrameTime();
        SetShaderValue(coolShader, rlGetLocationUniform(coolShader.id, "lightPos"), lightPosition,
                       SHADER_UNIFORM_VEC3);
    }
    if (IsKeyPressed(KEY_SPACE)) {
        printf("(%f/%f/%f)", lightPosition->x, lightPosition->y, lightPosition->z);
    }
}

struct Game {
    std::string playerName;
    int         playerAge;
    int         customers;
    float       customersCooldown;
    float       customersCooldownMax;
    int         smallCupsAmount;
    int         bigCupsAmount;
    int         beansAmount;
    int         grindedBeansAmount;
    int         cowMilkAmount;
    int         oatMilkAmount;
    int         madeEspressoAmount;
    int         madeLatteAmount;
    int         madeLongCafeAmount;
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
    SetTargetFPS(30);

    Vector3 objPosition = {0, 0, 0};
    Vector3 lightPosition = {7.0f, 4.0f, 6.0f};
    float shadowMapResolution = 512;
    Vector3 viewPos = {0, 0, 0};
    Vector3 viewDir = {0, 0, 0};
    float rotation = 90;
    float uniformScale = 1.25f;
    float time = 0;
    Model modelTarget = LoadModel("C:/Users/yui/Desktop/raylibLearning/CafeBar.glb");
    modelTarget.transform *= MatrixScale(uniformScale, uniformScale, uniformScale);
    RenderTexture screenTexture = LoadRenderTexture(1280, 720);
    Shader screenShader = LoadShader(0, TextFormat("C:/Users/yui/Desktop/raylibLearning/screenFragmentShader.glsl", 330));
    Shader shadowmapShader = LoadShader(TextFormat("C:/Users/yui/Desktop/raylibLearning/shadowmapVShader.glsl", 330),
                                        TextFormat("C:/Users/yui/Desktop/raylibLearning/shadowmapFShader.glsl", 330));
    Camera3D lightCam = Camera3D{0};
    lightCam.position = Vector3Scale(lightPosition, 1.0f);
    lightCam.target = Vector3{0,0,0};
    lightCam.projection = CAMERA_ORTHOGRAPHIC;
    lightCam.up = Vector3{0, 1, 0};
    lightCam.fovy = 25.0f;
    RenderTexture2D shadowTexture = LoadRenderTextureWithDepth(shadowMapResolution, shadowMapResolution);
    SetTextureWrap(shadowTexture.texture, TEXTURE_WRAP_CLAMP);
    SetTextureFilter(shadowTexture.texture, RL_TEXTURE_FILTER_LINEAR);

    Shader coolShader = LoadShader(TextFormat("C:/Users/yui/Desktop/raylibLearning/vertexShader.glsl", 330),
                                   TextFormat("C:/Users/yui/Desktop/raylibLearning/fragmentShader.glsl", 330));

    Texture2D colorMap = LoadTexture("C:/Users/yui/Desktop/raylibLearning/texture/colorMap.png");
    SetTextureFilter(colorMap, RL_TEXTURE_FILTER_LINEAR);
    SetTextureWrap(colorMap, TEXTURE_WRAP_REPEAT);

    SetTraceLogLevel(LOG_WARNING);

    //Load GameStruct
    Game game = Game();
    game.playerName = "olantern";
    game.playerAge = 28;
    game.customers = 0;
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

    while (!WindowShouldClose()) {
        game.customersCooldown-= GetFrameTime();
        
        ReadInput(&lightPosition, coolShader);
        lightCam.position = Vector3Scale( lightPosition, 1.0f);

        // shadowRender
        Matrix lightProj;
        Matrix lightView;
        for (size_t ma = 0; ma < modelTarget.materialCount; ++ma) {
            modelTarget.materials[ma].shader = shadowmapShader;
        }
        rlSetCullFace(rlCullMode::RL_CULL_FACE_BACK);
        rlSetClipPlanes(0.01f, 100.0f);
        BeginTextureMode(shadowTexture);
        {
            ClearBackground(RAYWHITE);
            BeginMode3D(lightCam);
            {
                lightView = rlGetMatrixModelview();
                lightProj = rlGetMatrixProjection();
                
                for (int mesh_index = 0; mesh_index < modelTarget.meshCount; ++mesh_index) {
                    DrawMesh(modelTarget.meshes[mesh_index], modelTarget.materials[modelTarget.meshMaterial[mesh_index]],
                             modelTarget.transform);
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
        //SetShaderValueTexture(coolShader, GetShaderLocation(coolShader, "shadow0"), shadowTexture.depth);

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
                    Texture roughnessMap =
                        modelTarget.materials[modelTarget.meshMaterial[mesh_index]].maps[MATERIAL_MAP_ROUGHNESS].texture;
                    Texture metalicMap =
                        modelTarget.materials[modelTarget.meshMaterial[mesh_index]].maps[MATERIAL_MAP_METALNESS].texture;

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
                        SetShaderValueTexture(coolShader, GetShaderLocation(coolShader, "shadow0"), shadowTexture.depth);

                    DrawMesh(modelTarget.meshes[mesh_index], modelTarget.materials[modelTarget.meshMaterial[mesh_index]],
                             modelTarget.transform);
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
                //DrawTexture(shadowTexture.texture, 0, 0, WHITE);
            }
            EndShaderMode();

            DrawRectangle(screenTexture.texture.width-50, 
                (screenTexture.texture.height-200.0f)/2, 
                30, 200, BLUE);
            DrawRectangle(screenTexture.texture.width - 50 + 4, 
                (screenTexture.texture.height - 200.0f) / 2 + 4, 
                30 - 8, 200 - 8, DARKBLUE);
            std::time_t currentClock = std::time(nullptr);
            char timebuffer[6];
            std::strftime(timebuffer, sizeof(timebuffer), "%H:%M", std::localtime(&currentClock));

            Rectangle buttonRect{30, 100, 100, 25};
            Rectangle labelRect{150, 100, 100, 25};
            if (GuiButton(buttonRect, "get Beans")) {
                printf("get Beans \n");
            }
            GuiLabel(labelRect, std::string("Beans: ").append(std::to_string(game.beansAmount)).c_str());
            buttonRect.y += 30;
            labelRect.y += 30;
            if (GuiButton(buttonRect, "grind beans")) {
                printf("grind beans \n");
            }
            GuiLabel(labelRect, std::string("Grind. beans: ").append(std::to_string(game.grindedBeansAmount)).c_str());
            buttonRect.y += 50;
            labelRect.y += 50;
            if (GuiButton(buttonRect, "Small Cups")) {
                printf("get small cups \n");
            }
            GuiLabel(labelRect, std::string("Small Cups: ").append(std::to_string(game.smallCupsAmount)).c_str());
            buttonRect.y += 30;
            labelRect.y += 30;
            if (GuiButton(buttonRect, "Large Cups")) {
                printf("Large Cups \n");
            }
            GuiLabel(labelRect, std::string("Large Cups: ").append(std::to_string(game.bigCupsAmount)).c_str());

            DrawText(timebuffer,5,5,48, BLACK);
        }
        EndDrawing();       
    }
    rlUnloadFramebuffer(shadowTexture.id);
    UnloadShader(coolShader);
    UnloadShader(shadowmapShader);
    UnloadModel(modelTarget);
    return 0;
}
