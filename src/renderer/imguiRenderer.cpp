#pragma once
#include "renderer.cpp"
#include "meshRenderer.cpp"
#include "../game.cpp"
#include "../config.cpp"

#include <__ios/fpos.h>
#include <string>

#include <imgui_impl_raylib.h>
#include <imgui.h>
#include <raylib.h>

class imguiRenderer : public renderer {
  public:
    imguiRenderer();
    ~imguiRenderer() override;

    void Init() override;
    void Loop() override;
    void Close() override;

    Game* game;
    meshRenderer* meshRenderer;
    RenderTexture2D* screenTexture;
    bool layer_opened[LAYER_MAX];

  private:
};

imguiRenderer::imguiRenderer() {}

imguiRenderer::~imguiRenderer() {}

void imguiRenderer::Init() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // tell ImGui the display scale
    if (!IsWindowState(FLAG_WINDOW_HIGHDPI))
    {
        io.DisplayFramebufferScale.x = GetWindowScaleDPI().x;
        io.DisplayFramebufferScale.y = GetWindowScaleDPI().y;
    }

    ImGui::StyleColorsDark();
    ImGui_ImplRaylib_Init();

    io.Fonts->AddFontFromFileTTF(config::getInstance().fontNormalPath.c_str());
    //io.Fonts->AddFontDefault(&defaultConfig);
}

void imguiRenderer::Loop() {
    ImGui_ImplRaylib_ProcessEvents();
    ImGui_ImplRaylib_NewFrame();
    ImGui::NewFrame();
    ImGui::DockSpaceOverViewport(ImGui::GetID("Lights"), NULL);

    ImGui::Begin("Lights");
    for(auto index = 0; index < game->scenes[0].lights.size(); ++index)
    {
        ImGui::Text("Light: \t %i", index);
        ImGui::PushID(index);
        ImGui::InputFloat3("Position", &game->scenes[0].lights[index].cam.position.x);
        ImGui::InputFloat3("Color", &game->scenes[0].lights[index].Color.x);
        ImGui::InputFloat("Amplitude", &game->scenes[0].lights[index].amplitude);
        ImGui::InputFloat3("Direction", &game->scenes[0].lights[index].direction.x);
        ImGui::PopID();
        ImGui::Spacing();
    }
    ImGui::End();

    ImGui::Begin("Game"); 
    ImVec2 gameWindowSize = ImGui::GetWindowContentRegionMax();
    gameWindowSize.x -= ImGui::GetWindowContentRegionMin().x;
    gameWindowSize.y -= ImGui::GetWindowContentRegionMin().y;

    ImVec2 gameWindowImageOffset{0,0};
    if(gameWindowSize.x < screenTexture->texture.width){
        gameWindowImageOffset.x = (screenTexture->texture.width - gameWindowSize.x)/2;
        gameWindowImageOffset.x /= screenTexture->texture.width;
    }
    if(gameWindowSize.y < screenTexture->texture.height){
        gameWindowImageOffset.y = (screenTexture->texture.height - gameWindowSize.y)/2;
        gameWindowImageOffset.y /= screenTexture->texture.height;
    }
    ImGui::Image((ImTextureID)screenTexture->texture.id, gameWindowSize, 
                    ImVec2(  gameWindowImageOffset.x, 1), 
                    ImVec2(1,   gameWindowImageOffset.y)); 
    ImGui::End();

    ImGui::Begin("Renderer");
    for(auto index = 0; index < game->lastaddedLayer; ++index)
    {
        RenderLayer& layer = game->layer[index];

        ImGui::BeginGroup();
        ImGui::PushID(index);
        if (ImGui::TreeNode(layer.name))
        {
            if(ImGui::BeginCombo("BlendMode", renderer::blendmodes[layer.blendMode].c_str()))
            {
                for(auto index = 0; index < 8; ++index){
                    if(ImGui::Selectable(renderer::blendmodes[index].c_str(),(index == layer.blendMode) )){
                        layer.blendMode = index;
                    }
                }
                ImGui::EndCombo();
            }
            if(ImGui::BeginCombo("CullMode", renderer::cullmodes[layer.cullMode].c_str()))
            {
                for(auto index = 0; index < 2; ++index){
                    if(ImGui::Selectable(renderer::cullmodes[index].c_str(),(index == layer.cullMode) )){
                        layer.cullMode = index;
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::Checkbox("ClearBackground", &layer.clearBackground);
            ImGui::InputInt4("Background", &layer.backgroundColor[0]);

            ImGui::TreePop();
        }
        ImGui::PopID();
        ImGui::EndGroup();
    }
    ImGui::End();

    ImGui::Render();
    ImGui_ImplRaylib_RenderDrawData(ImGui::GetDrawData());
}

void imguiRenderer::Close() {
}
