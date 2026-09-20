#pragma once
#include "renderer.cpp"
#include "../scene.cpp"
#include "../game.cpp"

#include <cstddef>
#include <iostream>
#include <ostream>
#include <chrono>

#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>

using namespace std::chrono;

class meshRenderer : public renderer{
public:
    meshRenderer();
    ~meshRenderer() override;

    void Init() override;
    void Loop() override;
    void Close() override;
    
    std::vector<RenderTexture2D> renTextures{};
    RenderTexture2D* resultTexture;
    Game* game = nullptr;
    Scene* scene = nullptr;

    Texture2D colorMap;
    float shadowMapResolution = 2048.0f;

    RenderTexture2D LoadRenderTextureWithDepth(int width, int height);
private:
    Material defaultMat = LoadMaterialDefault();
    float def_roughness = 0.4f;
    float def_metalic = 0.1f;
    float def_diffuse = 1;

    Vector3 viewPos{0,0,0};
    Vector3 viewDir{0,0,0};


    void renderDepth();
    void shadow_inLightMVP(int layerId);
    void shadow_ToCameraMVP(int layerId);
    void pbrLoop(int layerId, int lightIndex);
    void lightSphereLoop();
    void shadowToScreenTexture(int layerId);
    void RenderScreenTexture(int layerId);

    void updateObjectTransformByAnim(Vector3 *trans, int model_index, int frame);

    int frameCount{0};         //used for some benchmarking
    int frameCountModulo{30};   //update once frameCount == frameCountModulo

};

meshRenderer::meshRenderer() {
}

meshRenderer::~meshRenderer() {
}

void meshRenderer::Init(){
    renTextures.push_back( LoadRenderTextureWithDepth(1280, 720));
    if(game == nullptr) std::cout << "game nullptr" << std::endl;
    if(scene == nullptr) std::cout << "scene nullptr" << std::endl;
   
    for (auto &light : scene->lights) {
        std::cout << "light" << std::endl; 
        light.shadowTexture = LoadRenderTextureWithDepth(shadowMapResolution, shadowMapResolution);
        SetTextureWrap(light.shadowTexture.texture, TEXTURE_WRAP_CLAMP);
        SetTextureFilter(light.shadowTexture.texture, RL_TEXTURE_FILTER_LINEAR);
        light.shadowsCalculated = LoadRenderTexture(1280, 720);
        light.shadowsrendered = LoadRenderTexture(1280, 720);
    }
}

void meshRenderer::Loop(){
    auto startLoop = std::chrono::high_resolution_clock::now();
    for(int layerIndex = game->layer[0].nextNode; layerIndex != 0 ; layerIndex = game->layer[layerIndex].nextNode)
    {
        auto start = std::chrono::high_resolution_clock::now();
        switch(game->layer[layerIndex].nodeType)
        {
            case renderer::RenderState::null:
            {
            }break;
            case renderer::RenderState::cull:
            {
                rlEnableBackfaceCulling();
            }break;
            case renderer::RenderState::cullEnd:
            {
                rlDisableBackfaceCulling();
            }break;
            case renderer::RenderState::blend:
            {
                BeginBlendMode(game->layer[layerIndex].blendMode);
            }break;
            case renderer::RenderState::blendEnd:
            {
                EndBlendMode();
            }break;
            case renderer::RenderState::depth:
            {
                shadow_inLightMVP(layerIndex);
            }break;
            case renderer::RenderState::light:
            {

            }break;
            case renderer::RenderState::shadow:
            {
                shadow_ToCameraMVP(layerIndex);
            }break;
            case renderer::RenderState::shadowToScreen:
            {
                shadowToScreenTexture(layerIndex);
            }break;
            case renderer::RenderState::pbr:
            {
                //try to render for each light
                // light0 * shadow0
                //+light1 * shadow1
                //+light2 * shadow2
                rlCullMode(RL_CULL_FACE_BACK);
                //renderDepth();
                for(int index = 0; index < scene->lights.size(); ++index){
                    pbrLoop(layerIndex, index);
                }
                //auto framebuffer = rlLoadTextureDepth(resultTexture->texture.width, resultTexture->texture.height, true);

                rlEnableColorBlend();
                for(int index = 0; index < scene->lights.size(); ++index){
                    BeginTextureMode(scene->lights[index].shadowTexture);
                    ClearBackground(WHITE);
                    rlSetBlendMode(BlendMode::BLEND_ALPHA); 
                    DrawTextureRec(scene->lights[index].shadowsrendered.texture,
                                   Rectangle{0, 0, 
                                            (float)scene->lights[index].shadowsrendered.texture.width, 
                                            (float)-scene->lights[index].shadowsrendered.texture.height},
                                   Vector2{0, 0}, WHITE);

                    rlSetBlendFactorsSeparate(RL_DST_COLOR, RL_ZERO, RL_SRC_ALPHA, RL_ONE, RL_FUNC_ADD, RL_FUNC_ADD);
                    rlSetBlendMode(BlendMode::BLEND_CUSTOM_SEPARATE);

                    DrawTextureRec(scene->lights[index].shadowsCalculated.texture,
                                   Rectangle{0, 0, 
                                            (float)scene->lights[index].shadowsCalculated.texture.width, 
                                            (float)-scene->lights[index].shadowsCalculated.texture.height},
                                   Vector2{0, 0}, WHITE);

                    EndTextureMode();
                } 
                
                BeginTextureMode(*resultTexture);
                rlSetBlendFactorsSeparate(RL_ONE, RL_ONE, RL_ONE, RL_ZERO, RL_FUNC_ADD, RL_FUNC_ADD);
                ClearBackground(BLACK);
                rlSetBlendMode(BlendMode::BLEND_CUSTOM_SEPARATE);
                for(int index = 0; index < scene->lights.size(); ++index){
                    DrawTextureRec(scene->lights[index].shadowTexture.texture,
                                   Rectangle{0, 0, 
                                            (float)scene->lights[index].shadowTexture.texture.width, 
                                            (float)-scene->lights[index].shadowTexture.texture.height},
                                   Vector2{0, 0}, WHITE);
                }
                /*
                rlSetBlendFactorsSeparate(RL_DST_COLOR, RL_ZERO, RL_ONE, RL_ZERO, RL_FUNC_ADD, RL_FUNC_ADD);
                rlSetBlendMode(BlendMode::BLEND_CUSTOM_SEPARATE);
                lightSphereLoop();
                */
                rlSetBlendMode(BlendMode::BLEND_ALPHA);

                EndTextureMode();


                //EndTextureMode();
            }break;
            case renderer::RenderState::screen:
            {
                RenderScreenTexture(layerIndex);
            }break;
        }
        auto end = std::chrono::high_resolution_clock::now();
        game->layer[layerIndex].calctimeDuration += duration_cast<std::chrono::milliseconds>(end-start).count();
        if(frameCount == frameCountModulo){
            game->layer[layerIndex].timeDuration = game->layer[layerIndex].calctimeDuration / frameCountModulo; 
            game->layer[layerIndex].calctimeDuration = 0;
            frameCount = 0;
        }else{
            frameCount++;
        }
    }
    auto endLoop = std::chrono::high_resolution_clock::now();
    game->layer[0].calctimeDuration += std::chrono::duration_cast<std::chrono::milliseconds>(endLoop-startLoop).count();
    game->layer[0].timeDuration = game->layer[0].calctimeDuration / frameCountModulo; 
}

void meshRenderer::Close(){

}

void meshRenderer::shadow_inLightMVP(int layerid)
{
    // shadowRender
    rlEnableBackfaceCulling();
    rlSetCullFace((bool)game->layer[layerid].cullMode);
    Matrix lightProj;
    Matrix lightView;
    Shader shadowmapShader = game->layer[layerid].shader;
    defaultMat.shader = shadowmapShader;
    for (size_t gO = 0; gO < scene->gameObjects.size(); ++gO) {
        for (int anim_index = 0; anim_index < scene->gameObjects[gO].animations.size(); ++anim_index) {
            scene->gameObjects[gO].animationsFrame += 1;
            if (scene->gameObjects[gO].animationsFrame >= scene->gameObjects[gO].animations[anim_index].keyframeCount)
                scene->gameObjects[gO].animationsFrame = 0;
        }
        for (size_t ma = 0; ma < scene->gameObjects[gO].model.materialCount; ++ma) {
            scene->gameObjects[gO].model.materials[ma].shader = shadowmapShader;
        }
    }
    rlSetBlendMode(game->layer[layerid].blendMode);
    rlEnableColorBlend();
    for(size_t lightIndex = 0; lightIndex < scene->lights.size(); ++lightIndex)
    {
        Light &light = scene->lights.at(lightIndex);
        
        BeginTextureMode(light.shadowTexture);
        {
            if (game->layer[layerid].clearBackground){
                ClearBackground(Color{
                    (unsigned char)game->layer[layerid].backgroundColor[0],
                    (unsigned char)game->layer[layerid].backgroundColor[1],
                    (unsigned char)game->layer[layerid].backgroundColor[2],
                    (unsigned char)game->layer[layerid].backgroundColor[3]});
            }
            BeginMode3D(light.cam);
            {
                lightView = rlGetMatrixModelview();
                lightProj = rlGetMatrixProjection();
                light.projectionMatrix = MatrixMultiply(lightView, lightProj);

                SetShaderValue(shadowmapShader, rlGetLocationUniform(shadowmapShader.id, "minclamp"), &scene->clipRange.x, SHADER_UNIFORM_FLOAT);
                SetShaderValue(shadowmapShader, rlGetLocationUniform(shadowmapShader.id, "maxclamp"), &scene->clipRange.y, SHADER_UNIFORM_FLOAT);
                SetShaderValueMatrix(shadowmapShader, GetShaderLocation(shadowmapShader, "matLightVP"), light.projectionMatrix);

                for (int model_index = 0; model_index < scene->gameObjects.size(); ++model_index) {
                    if (scene->gameObjects[model_index].depthIgnore && false)
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
        EndTextureMode();
    }
    rlDisableColorBlend();
    rlDisableBackfaceCulling();
    rlSetCullFace(!(bool)game->layer[layerid].cullMode);
}

void meshRenderer::renderDepth()
{
    BeginMode3D(scene->mainCam);
    {
        for (size_t gO = 0; gO < scene->gameObjects.size(); ++gO) {
            Vector3 trans = scene->gameObjects[gO].translation;
            auto &model = scene->gameObjects[gO].model;
            DrawModel(model, trans, 1.0f, WHITE);
        }
    }
    EndMode3D();
}

void meshRenderer::shadow_ToCameraMVP(int layerId)
{
    viewPos = scene->mainCam.position;
    viewDir = Vector3Normalize(scene->mainCam.target - viewPos);
    Shader coolShader = game->layer[layerId].shader;

    RenderTexture screenTexture = renTextures.at(0);
    defaultMat.shader = coolShader;
    for (size_t gO = 0; gO < scene->gameObjects.size(); ++gO) {
        Vector3 trans = scene->gameObjects[gO].translation;
        for (size_t ma = 0; ma < scene->gameObjects[gO].model.materialCount; ++ma) {
            scene->gameObjects[gO].model.materials[ma].shader = coolShader;
        }
    }
    //rlEnableBackfaceCulling();
    rlSetCullFace(game->layer[layerId].cullMode);

    for(size_t lightIndex = 0; lightIndex < scene->lights.size(); ++lightIndex)
    {
        Light &light = scene->lights[lightIndex];
        BeginTextureMode(light.shadowsCalculated);
            if(game->layer[layerId].clearBackground)
            {
                ClearBackground(Color{
                    (unsigned char)game->layer[layerId].backgroundColor[0],
                    (unsigned char)game->layer[layerId].backgroundColor[1],
                    (unsigned char)game->layer[layerId].backgroundColor[2],
                    (unsigned char)game->layer[layerId].backgroundColor[3]});
            }
            BeginMode3D(scene->mainCam);
            
            SetShaderValueMatrix(coolShader,GetShaderLocation(coolShader, "lightViewProj"), light.projectionMatrix);
            SetShaderValue(coolShader,      rlGetLocationUniform(coolShader.id, "lightPos"), &light.cam.position, SHADER_UNIFORM_VEC3);
            SetShaderValue(coolShader,      rlGetLocationUniform(coolShader.id, "lightColor"), &light.Color, SHADER_UNIFORM_VEC3);

            int shadowMapRes_Location = rlGetLocationUniform(coolShader.id, "shadowMapResolution");
            if(shadowMapRes_Location == -1) game->layer[layerId].error = true;
            SetShaderValue(coolShader, shadowMapRes_Location, &shadowMapResolution, SHADER_UNIFORM_FLOAT);

            int amplitude_Location = GetShaderLocation(coolShader, "amplitude");
            SetShaderValue(coolShader, amplitude_Location, &light.amplitude, SHADER_UNIFORM_FLOAT);
            if(amplitude_Location == -1) game->layer[layerId].error = true;

            auto shadow0loc = GetShaderLocation(coolShader, "shadow0");
            SetShaderValueTexture(coolShader,GetShaderLocation( coolShader,    "shadow0"), light.shadowTexture.depth);
            coolShader.locs[1] = shadow0loc;

            BeginShaderMode(coolShader);
            {
                for (size_t gO = 0; gO < scene->gameObjects.size(); ++gO) {
                    Vector3 trans = scene->gameObjects[gO].translation;
                    auto &model = scene->gameObjects[gO].model;
                    for (size_t meshIndex = 0; meshIndex < model.meshCount; ++meshIndex) {
                        //model.materials[model.meshMaterial[meshIndex]].shader.locs[1] = shadow0loc;
                        model.materials[model.meshMaterial[meshIndex]].maps[1].texture = light.shadowTexture.depth;
                        DrawMesh(model.meshes[meshIndex], model.materials[model.meshMaterial[meshIndex]], model.transform);
                    }
                }
            }
            EndShaderMode();
            EndMode3D();
        EndTextureMode();
    }
    EndBlendMode();
    rlDisableColorBlend();

}

void meshRenderer::shadowToScreenTexture(int layerId){
    RenderTexture& screenTexture = *resultTexture;
    ClearBackground(WHITE);
    rlEnableColorBlend();
    rlSetBlendMode(BLEND_MULTIPLIED);
    BeginTextureMode(screenTexture);
        for (size_t lightIndex = 0; lightIndex < scene->lights.size(); ++lightIndex) {
            Texture &shadowsCalculated = scene->lights[lightIndex].shadowsCalculated.texture;
            DrawTextureRec(shadowsCalculated,
                           Rectangle{0, 0, (float)shadowsCalculated.width, (float)-shadowsCalculated.height},
                           Vector2{0, 0}, WHITE);
        }
    EndTextureMode();
    rlClearScreenBuffers();
    rlDisableColorBlend();
}

void meshRenderer::RenderScreenTexture(int layerId){
    Shader screenShader = game->layer[layerId].shader;
    //RenderTexture screenTexture = renTextures.at(0);
    RenderTexture& screenTexture = *resultTexture;

    // screentexture
    //ClearBackground(game->layer[layerid].backgroundColor);
    if (game->layer[layerId].clearBackground){
        rlClearScreenBuffers();
        ClearBackground(Color{
            (unsigned char)game->layer[layerId].backgroundColor[0],
            (unsigned char)game->layer[layerId].backgroundColor[1],
            (unsigned char)game->layer[layerId].backgroundColor[2],
            (unsigned char)game->layer[layerId].backgroundColor[3]});
    }

    BeginShaderMode(screenShader);
    {
        //RenderTexture &screenTexture = scene->lights[lightIndex].shadowsCalculated;
        SetShaderValueTexture(screenShader, rlGetLocationUniform(screenShader.id, "colormap"), colorMap);
        DrawTextureRec(screenTexture.texture,
                       Rectangle{0, 0, (float)screenTexture.texture.width, (float)-screenTexture.texture.height},
                       Vector2{0, 0}, WHITE);
    }
    EndShaderMode();
}

void meshRenderer::pbrLoop(int layerId, int lightIndex)
{
    Shader pbrShader = game->layer[layerId].shader;
    Vector3 viewdir = Vector3Normalize(scene->mainCam.target - scene->mainCam.position);
    SetShaderValue(pbrShader, GetShaderLocation(pbrShader, "viewPos"), &scene->mainCam.position, SHADER_UNIFORM_VEC3);
    SetShaderValue(pbrShader, GetShaderLocation(pbrShader, "viewDir"), &viewDir, SHADER_UNIFORM_VEC3);
    SetShaderValue(pbrShader, GetShaderLocation(pbrShader, "lightPos"), &scene->lights[lightIndex].cam.position, SHADER_UNIFORM_VEC3);
    SetShaderValue(pbrShader, GetShaderLocation(pbrShader, "lightColor"), &scene->lights[lightIndex].Color, SHADER_UNIFORM_VEC3);
    SetShaderValue(pbrShader, GetShaderLocation(pbrShader, "lightstrength"), &scene->lights[lightIndex].amplitude, SHADER_UNIFORM_FLOAT);

    BeginTextureMode(scene->lights[lightIndex].shadowsrendered);
    BeginMode3D(scene->mainCam);
    BeginShaderMode(pbrShader);
    {
        if (game->layer[layerId].clearBackground){
            ClearBackground(Color{
                (unsigned char)game->layer[layerId].backgroundColor[0],
                (unsigned char)game->layer[layerId].backgroundColor[1],
                (unsigned char)game->layer[layerId].backgroundColor[2],
                (unsigned char)game->layer[layerId].backgroundColor[3]});
        }
        for (size_t gO = 0; gO < scene->gameObjects.size(); ++gO) {
            Vector3 trans = scene->gameObjects[gO].translation;
            auto &model = scene->gameObjects[gO].model;
            for (size_t meshIndex = 0; meshIndex < model.meshCount; ++meshIndex) {
                model.materials[model.meshMaterial[meshIndex]].shader = pbrShader;
                DrawMesh(model.meshes[meshIndex], model.materials[model.meshMaterial[meshIndex]], model.transform);
            }
        }
        lightSphereLoop();
    }
    EndShaderMode();
    EndMode3D();

    EndBlendMode();
    EndTextureMode();
}

void meshRenderer::lightSphereLoop(){
    for(size_t lightIndex = 0; lightIndex < scene->lights.size(); ++lightIndex)
    {
        Light &light = scene->lights[lightIndex];
        DrawSphere(scene->lights[lightIndex].cam.position, 0.2f, BLUE);
    }
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
        rlTextureParameters(target.depth.id, RL_TEXTURE_WRAP_S, RL_TEXTURE_WRAP_CLAMP);
        rlTextureParameters(target.depth.id, RL_TEXTURE_WRAP_T, RL_TEXTURE_WRAP_CLAMP);

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
        scene->gameObjects[model_index].animations[scene->gameObjects[model_index].animationCurrent].keyframePoses[0]->translation.x / 1;
    trans->y -=
        scene->gameObjects[model_index].animations[scene->gameObjects[model_index].animationCurrent].keyframePoses[0]->translation.z / 1;
    trans->z -=
        scene->gameObjects[model_index].animations[scene->gameObjects[model_index].animationCurrent].keyframePoses[0]->translation.y /-1;

    trans->x += scene->gameObjects[model_index]
                   .animations[scene->gameObjects[model_index].animationCurrent]
                   .keyframePoses[(int)frame]
                   ->translation.x /
               1;
    trans->y += scene->gameObjects[model_index]
                   .animations[scene->gameObjects[model_index].animationCurrent]
                   .keyframePoses[(int)frame]
                   ->translation.z /
               1;
    trans->z += scene->gameObjects[model_index]
                   .animations[scene->gameObjects[model_index].animationCurrent]
                   .keyframePoses[(int)frame]
                   ->translation.y /
               -1;
}
