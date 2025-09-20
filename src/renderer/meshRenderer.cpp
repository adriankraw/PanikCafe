#include "renderer.cpp"
#include "../scene.cpp"
#include "../game.cpp"

#include <iostream>
#include <ostream>
#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
#include <map>



class meshRenderer : public renderer{
public:
    meshRenderer();
    ~meshRenderer() override;

    void Init() override;
    void Loop() override;
    void Close() override;
    
    renderer::RenderState currentState = RenderState::light;
    std::vector<RenderTexture2D> renTextures{};
    Game* game = nullptr;
    Scene* scene = nullptr;

    Texture2D colorMap;
    int shadowMapResolution = 512;


private:
    Material defaultMat = LoadMaterialDefault();
    float def_roughness = 0.4f;
    float def_metalic = 0.1f;
    float def_diffuse = 1;

    Vector3 viewPos{0,0,0};
    Vector3 viewDir{0,0,0};

    void shadowLoop();
    void lightLoop();
    void screenLoop();

    void updateObjectTransformByAnim(Vector3 *trans, int model_index, int frame);

    RenderTexture2D LoadRenderTextureWithDepth(int width, int height);
};

meshRenderer::meshRenderer() {
}

meshRenderer::~meshRenderer() {
}

void meshRenderer::Init(){
    renTextures.push_back( LoadRenderTextureWithDepth(shadowMapResolution, shadowMapResolution));
    if(game == nullptr) std::cout << "game nullptr" << std::endl;
    if(scene == nullptr) std::cout << "scene nullptr" << std::endl;
    
    for (auto &light : scene->lights) {
        std::cout << "light" << std::endl; 
        light.shadowTexture = LoadRenderTextureWithDepth(shadowMapResolution, shadowMapResolution);
        SetTextureWrap(light.shadowTexture.texture, TEXTURE_WRAP_CLAMP);
        SetTextureFilter(light.shadowTexture.texture, RL_TEXTURE_FILTER_LINEAR);
    }
}

void meshRenderer::Loop(){
    switch(currentState)
    {
        case RenderState::light:
            lightLoop();
            break;
        case RenderState::shadow:
            shadowLoop();
            break;
        case RenderState::screen:
            screenLoop();
            break;
    }
}

void meshRenderer::Close(){

}

void meshRenderer::shadowLoop()
{
    // shadowRender
    rlDisableBackfaceCulling();
    rlSetCullFace(rlCullMode::RL_CULL_FACE_FRONT);
    Matrix lightProj;
    Matrix lightView;
    Shader shadowmapShader = shader[RenderState::shadow];
    defaultMat.shader = shadowmapShader;
    for (size_t gO = 0; gO < scene->gameObjects.size(); ++gO) {
        for (int anim_index = 0; anim_index < scene->gameObjects[gO].animations.size(); ++anim_index) {
            scene->gameObjects[gO].animationsFrame += 1;
            if (scene->gameObjects[gO].animationsFrame >= scene->gameObjects[gO].animations[anim_index].frameCount)
                scene->gameObjects[gO].animationsFrame = 0;
        }
        for (size_t ma = 0; ma < scene->gameObjects[gO].model.materialCount; ++ma) {
            scene->gameObjects[gO].model.materials[ma].shader = shadowmapShader;
        }
    }
    std::cout << game->scenes.back().gameObjects.size() << std::endl; 
    for (auto &light : scene->lights) {
        SetShaderValue(shadowmapShader, rlGetLocationUniform(shadowmapShader.id, "lightPos"), &light.cam.position,
                       SHADER_UNIFORM_VEC3);
        //BeginTextureMode(light.shadowTexture);
        BeginDrawing();
        {
            ClearBackground(RAYWHITE);
            BeginMode3D(light.cam);
            {
                lightView = rlGetMatrixModelview();
                lightProj = rlGetMatrixProjection();
                for (int model_index = 0; model_index < scene->gameObjects.size(); ++model_index) {
                    if (scene->gameObjects[model_index].depthIgnore)
                        continue;

                    Vector3 trans = scene->gameObjects[model_index].translation;
                    if (scene->gameObjects[model_index].animations.size() > 0) {
                        float frame = scene->gameObjects[model_index].animationsFrame;
                        updateObjectTransformByAnim(&trans, model_index, frame);
                    }
                    DrawModelEx(scene->gameObjects[model_index].model, trans, Vector3{0, 0, 0}, 1.0f, Vector3{1, 1, 1}, WHITE);
                }
                for (size_t cu = 0; cu < game->customers.size(); ++cu) {
                    if (game->customers[cu].active) {
                        float clicked = game->customers[cu].clicked ? 1 : 0;
                        DrawMesh(*game->customers[cu].mesh, defaultMat, scene->customersSpawnPoints.transform);
                    }
                }
            }
            EndMode3D();
        }
        EndDrawing();
        //EndTextureMode();
        light.projectionMatrix = MatrixMultiply(lightView, lightProj);
        break;
    }
}

void meshRenderer::lightLoop()
{
    RenderTexture screenTexture = renTextures.at(0);
    // light & color
    viewPos = scene->mainCam.position;
    viewDir = Vector3Normalize(scene->mainCam.target - viewPos);
    Shader coolShader = shader[RenderState::light];
    SetShaderValueV(coolShader, rlGetLocationUniform(coolShader.id, "viewPos"), &viewPos, SHADER_UNIFORM_VEC3, 1);
    SetShaderValueV(coolShader, rlGetLocationUniform(coolShader.id, "viewDir"), &viewDir, SHADER_UNIFORM_VEC3, 1);

    int lightIndex = 0;
    RenderTexture2D *shadowTexture = &scene->lights.at(lightIndex).shadowTexture;
    for (auto &light : scene->lights) {
        SetShaderValueV(coolShader, rlGetLocationUniform(coolShader.id, "lightPos") + lightIndex,
                        &scene->lights.at(lightIndex).cam.position, SHADER_UNIFORM_VEC3, 1);
        SetShaderValueMatrix(coolShader, rlGetLocationUniform(coolShader.id, "lightViewProj") + lightIndex,
                             scene->lights.at(lightIndex).projectionMatrix);
        lightIndex++;
    }

    defaultMat.shader = coolShader;
    for (size_t gO = 0; gO < scene->gameObjects.size(); ++gO) {
        for (size_t ma = 0; ma < scene->gameObjects[gO].model.materialCount; ++ma) {
            scene->gameObjects[gO].model.materials[ma].shader = coolShader;
        }
    }
    // rlEnableBackfaceCulling();
    rlSetCullFace(rlCullMode::RL_CULL_FACE_FRONT);
    BeginTextureMode(screenTexture);
    {
        ClearBackground(RAYWHITE);

        BeginMode3D(scene->mainCam);
        {
            for (size_t gO = 0; gO < scene->gameObjects.size(); ++gO) {
                scene->gameObjects[gO].model.materials->shader.locs[SHADER_LOC_MAP_DIFFUSE] =
                    scene->gameObjects[gO]
                        .model.materials[scene->gameObjects[gO].model.meshMaterial[0]]
                        .maps[MATERIAL_MAP_DIFFUSE]
                        .texture.id;
                if (scene->gameObjects[gO].model.materials->shader.locs[SHADER_LOC_MAP_DIFFUSE] <= 0) {
                    SetShaderValue(coolShader, GetShaderLocation(coolShader, "noTexture0"), &def_diffuse,
                                   SHADER_UNIFORM_FLOAT);
                }
                scene->gameObjects[gO].model.materials->shader.locs[SHADER_LOC_MAP_ROUGHNESS] =
                    scene->gameObjects[gO]
                        .model.materials[scene->gameObjects[gO].model.meshMaterial[0]]
                        .maps[MATERIAL_MAP_ROUGHNESS]
                        .texture.id;
                if (scene->gameObjects[gO].model.materials->shader.locs[SHADER_LOC_MAP_ROUGHNESS] <= 0) {
                    SetShaderValue(coolShader, GetShaderLocation(coolShader, "roughness1"), &def_roughness,
                                   SHADER_UNIFORM_FLOAT);
                }
                scene->gameObjects[gO].model.materials->shader.locs[SHADER_LOC_MAP_METALNESS] =
                    scene->gameObjects[gO]
                        .model.materials[scene->gameObjects[gO].model.meshMaterial[0]]
                        .maps[MATERIAL_MAP_METALNESS]
                        .texture.id;
                if (scene->gameObjects[gO].model.materials->shader.locs[SHADER_LOC_MAP_METALNESS] <= 0) {
                    SetShaderValue(coolShader, GetShaderLocation(coolShader, "metalic1"), &def_metalic,
                                   SHADER_UNIFORM_FLOAT);
                }

                if (shadowTexture->depth.id)
                    SetShaderValueTexture(coolShader, GetShaderLocation(coolShader, "shadow0"), shadowTexture->depth);

                for (int model_index = 0; model_index < scene->gameObjects.size(); ++model_index) {
                    Vector3 trans = scene->gameObjects[model_index].translation;
                    if (scene->gameObjects[model_index].animations.size() > 0) {
                        float frame = scene->gameObjects[model_index].animationsFrame;
                        updateObjectTransformByAnim(&trans, model_index, frame);
                    }

                    DrawModelEx(scene->gameObjects[model_index].model, trans, Vector3{0, 0, 0}, 1.0f, Vector3{1, 1, 1}, WHITE);
                }
            }
            float clicked = 0;
            for (size_t cu = 0; cu < game->customers.size(); ++cu) {
                if (game->customers[cu].active) {
                    clicked = game->customers[cu].clicked ? 1 : 0;
                    SetShaderValue(coolShader, GetShaderLocation(coolShader, "selected"), &clicked,
                                   SHADER_UNIFORM_FLOAT);
                    DrawMesh(*game->customers[cu].mesh, defaultMat, scene->customersSpawnPoints.transform);
                }
            }
            clicked = 0;
            SetShaderValue(coolShader, GetShaderLocation(coolShader, "selected"), &clicked, SHADER_UNIFORM_FLOAT);
        }
        // Draw lightSphere
        for (auto light : scene->lights) {
            DrawSphere(light.cam.position, 0.2f, BLUE);
        }
        EndMode3D();
    }
    EndTextureMode();
}

void meshRenderer::screenLoop()
{
    Shader screenShader = shader[RenderState::screen];
    RenderTexture screenTexture = renTextures.at(0);
    
    // screentexture
    BeginDrawing();
    {
        ClearBackground(RAYWHITE);

        BeginShaderMode(screenShader);
        {
            SetShaderValueTexture(screenShader, rlGetLocationUniform(screenShader.id, "colormap"), colorMap);
            DrawTextureRec(screenTexture.texture,
                           Rectangle{0, 0, (float)screenTexture.texture.width, (float)-screenTexture.texture.height},
                           Vector2{0, 0}, WHITE);
        }
        EndShaderMode();
    }
    EndDrawing();
}

RenderTexture2D meshRenderer::LoadRenderTextureWithDepth(int width, int height) {
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
void meshRenderer::updateObjectTransformByAnim(Vector3 *trans, int model_index, int frame) {
    trans->x -=
        scene->gameObjects[model_index].animations[scene->gameObjects[model_index].animationCurrent].framePoses[0]->translation.x / 1;
    trans->y -=
        scene->gameObjects[model_index].animations[scene->gameObjects[model_index].animationCurrent].framePoses[0]->translation.z / 1;
    trans->z -=
        scene->gameObjects[model_index].animations[scene->gameObjects[model_index].animationCurrent].framePoses[0]->translation.y /-1;

    trans->x += scene->gameObjects[model_index]
                   .animations[scene->gameObjects[model_index].animationCurrent]
                   .framePoses[(int)frame]
                   ->translation.x /
               1;
    trans->y += scene->gameObjects[model_index]
                   .animations[scene->gameObjects[model_index].animationCurrent]
                   .framePoses[(int)frame]
                   ->translation.z /
               1;
    trans->z += scene->gameObjects[model_index]
                   .animations[scene->gameObjects[model_index].animationCurrent]
                   .framePoses[(int)frame]
                   ->translation.y /
               -1;
}
