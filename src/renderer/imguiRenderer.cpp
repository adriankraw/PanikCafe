#pragma once
#include "renderer.cpp"
#include "meshRenderer.cpp"
#include "../game.cpp"
#include "../config.cpp"
#include <fstream>
#include <iomanip>

#ifdef __APPLE__
    #include <__ios/fpos.h>
#endif

#include <cstring>
#include <string>

#include <imgui_impl_raylib.h>
#include <imgui.h>
#include <raylib.h>
#include <json.hpp>

class imguiRenderer : public renderer {
  public:
    imguiRenderer();
    ~imguiRenderer() override;

    void Init() override;
    void Loop() override;
    void Close() override;

    Game* game;
    RenderTexture2D* screenTexture;
    bool layer_opened[LAYER_MAX];

    bool ishovered = false;

  private:
    void ImageCenter(ImVec2 viewportSize, int& width, int& height);
};

imguiRenderer::imguiRenderer() {}

imguiRenderer::~imguiRenderer() {}

void imguiRenderer::Init() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // tell ImGui the display scale
    if (!IsWindowState(FLAG_WINDOW_HIGHDPI))
    {
        io.DisplayFramebufferScale.x = GetWindowScaleDPI().x;
        io.DisplayFramebufferScale.y = GetWindowScaleDPI().y;
    }

    ImGui_ImplRaylib_Init();

    ImGui::StyleColorsLight();
    ImGuiStyle& style = ImGui::GetStyle();
    style.FontScaleMain = 0.8f;
    style.TabRounding = 0.1f;
    style.WindowMenuButtonPosition = ImGuiDir_None;
    style.WindowRounding = 5.0f;
    style.ScaleAllSizes(0.5f);

    io.Fonts->AddFontFromFileTTF(config::getInstance().fontNormalPath.c_str());
    
    //io.Fonts->AddFontDefault(&defaultConfig);
}

void imguiRenderer::Loop() {
    ImGui_ImplRaylib_ProcessEvents();
    ImGui_ImplRaylib_NewFrame();

    ishovered = ImGui::GetIO().WantCaptureMouse;

    ImGui::NewFrame();
    ImGui::DockSpaceOverViewport(ImGui::GetID("Lights"), NULL);
    
    ImGui::Begin("Hierarchy");
    {
        ImGuiTreeNodeFlags treeNodeflag = ImGuiTreeNodeFlags_DefaultOpen;
        treeNodeflag |= ImGuiTreeNodeFlags_Framed;
        treeNodeflag |= ImGuiTreeNodeFlags_Leaf;
        for(auto index = 0; index < game->scenes[0].gameObjects.size(); ++index)
        {
            std::string name = game->scenes[0].gameObjects[index].name;
            size_t nameCharIndex = 0;
            nameCharIndex = name.find_last_of('/');
            if(nameCharIndex < 0){
                nameCharIndex = name.find_last_of('\\');
            }
            name = name.substr(nameCharIndex+1);

            ImGui::PushID(index);
            if(ImGui::TreeNodeEx(name.c_str(), treeNodeflag)){
                ImGui::InputFloat3("Pos", &game->scenes[0].gameObjects[index].translation.x);
                ImGui::InputFloat3("Scale", &game->scenes[0].gameObjects[index].scaling.x);
                ImGui::InputFloat3("Rota", &game->scenes[0].gameObjects[index].rotation.x);
                ImGui::Checkbox("Ignore for Depth", &game->scenes[0].gameObjects[index].depthIgnore);
                ImGui::TreePop();
            }
            ImGui::PopID();
            ImGui::Spacing();
        }
    }
    ImGui::End();

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
    {
        auto viewportSize = ImGui::GetContentRegionAvail();
        auto imageWidth = screenTexture->texture.width;
        auto imageHeight = screenTexture->texture.height;
        ImageCenter(viewportSize, imageWidth, imageHeight);
        //

        ImGui::Image((ImTextureID)screenTexture->texture.id, ImVec2(imageWidth, imageHeight), 
                        ImVec2(0, 1), 
                        ImVec2(1, 0)); 

        ishovered = ishovered && ImGui::IsItemHovered();
    }
    ImGui::End();

    for(size_t i = 0; i < game->scenes[0].lights.size(); ++i)
    {
        std::string name = "LightDepth" + std::to_string(i);
        ImGui::Begin(name.c_str()); 
        auto viewportSize = ImGui::GetContentRegionAvail();
        auto imageWidth = game->scenes[0].lights[i].shadowTexture.texture.width;
        auto imageHeight = game->scenes[0].lights[i].shadowTexture.texture.width;
        ImageCenter(viewportSize, imageWidth,imageHeight);
        ImGui::Image((ImTextureID)game->scenes[0].lights[i].shadowTexture.texture.id, ImVec2(imageWidth, imageHeight), 
                        ImVec2(0, 1), 
                        ImVec2(1, 0)); 
        //ImGui::SetCursorPos(ImVec2(0,0));
        ImGui::End();
    }

    for(size_t i = 0; i < game->scenes[0].lights.size(); ++i)
    {
        std::string name = "LightCalc" + std::to_string(i);
        ImGui::Begin(name.c_str()); 
        auto viewportSize = ImGui::GetContentRegionAvail();
        auto imageWidth = game->scenes[0].lights[i].shadowsCalculated.texture.width;
        auto imageHeight = game->scenes[0].lights[i].shadowsCalculated.texture.width;
        ImageCenter(viewportSize, imageWidth,imageHeight);
        ImGui::Image((ImTextureID)game->scenes[0].lights[i].shadowsCalculated.texture.id, ImVec2(imageWidth, imageHeight), 
                        ImVec2(0, 1), 
                        ImVec2(1, 0)); 
        ImGui::End();
    }

    
    ImGuiTreeNodeFlags treeNodeflag = ImGuiTreeNodeFlags_DefaultOpen;
    treeNodeflag |= ImGuiTreeNodeFlags_Framed;
    treeNodeflag |= ImGuiTreeNodeFlags_Leaf;
    ImGui::Begin("Renderer");
    ImGui::BeginGroup();
    if(ImGui::SmallButton("Load/relaod Shader")){
        using json = nlohmann::json;
        std::fstream f("./Assets/shader.save");
        json shadersettings = json::parse(f);
        if(shadersettings.is_array()){
            //lösche den derzeitigen game->layer array 
            //resette game->lastlayerId
            game->lastaddedLayer = 0;
            for(auto& it :shadersettings){
                std::string name = it["name"];
                if(name.length()<64){
                    std::strcpy(game->layer[game->lastaddedLayer].name, name.c_str());
                    game->layer[game->lastaddedLayer].name;
                }
                game->layer[game->lastaddedLayer].nextNode = it["nextNode"];
                game->layer[game->lastaddedLayer].prevNode = it["prevNode"];
                game->layer[game->lastaddedLayer].nodeType = it["nodeType"];
                game->layer[game->lastaddedLayer].clearBackground = it["clearBackground"];
                game->layer[game->lastaddedLayer].cullMode = it["cullMode"];
                game->layer[game->lastaddedLayer].blendMode = it["blendMode"];
                game->layer[game->lastaddedLayer].backgroundColor[0] = it["backgroundColor"][0];
                game->layer[game->lastaddedLayer].backgroundColor[1] = it["backgroundColor"][1];
                game->layer[game->lastaddedLayer].backgroundColor[2] = it["backgroundColor"][2];
                game->layer[game->lastaddedLayer].backgroundColor[3] = it["backgroundColor"][3];
                game->layer[game->lastaddedLayer].shaderVertex = it["shaderVertex"];
                game->layer[game->lastaddedLayer].shaderFragment = it["shaderFragment"];
                game->layer[game->lastaddedLayer].reloadShader = true;
                game->lastaddedLayer++;
                //initialisiere den ganzen shader zeug
                
            }
            for(size_t l = game->lastaddedLayer; l < LAYER_MAX ; ++l){
                game->layer[game->lastaddedLayer].nextNode = 0;
                game->layer[game->lastaddedLayer].prevNode = 0;
                game->layer[game->lastaddedLayer].nodeType = 0;
            }
        }else{
        }

        f.close();
    }
    ImGui::SameLine();
    if(ImGui::SmallButton("saveShader")){
        using json = nlohmann::json;
        std::ofstream f("./Assets/shader.save");
        json j;
        for(size_t i = 0; i <= game->lastaddedLayer; ++i){
            json layerjson;
            layerjson["name"] = game->layer[i].name;
            layerjson["nextNode"] = game->layer[i].nextNode;
            layerjson["prevNode"] = game->layer[i].prevNode;
            layerjson["nodeType"] = game->layer[i].nodeType;
            layerjson["clearBackground"] = game->layer[i].clearBackground;
            layerjson["cullMode"] = game->layer[i].cullMode;
            layerjson["blendMode"] = game->layer[i].blendMode;
            layerjson["backgroundColor"] = game->layer[i].backgroundColor;
            layerjson["shaderVertex"] = game->layer[i].shaderVertex;
            layerjson["shaderFragment"] = game->layer[i].shaderFragment;
            j.push_back(layerjson);
        }

        f << std::setw(4)<< j << std::endl;
        f.close();
    }
    ImGui::EndGroup();
    for(int index = game->layer[0].nextNode; index != 0 ; index = game->layer[index].nextNode)
    {
        RenderLayer& layer = game->layer[index];

        ImGui::BeginGroup();
        ImGui::PushID(index);
        if(index != 0 && index != 100 && ImGui::Button("-", ImVec2(25,0.0f)))
        {
            int prevNode = game->layer[index].prevNode;
            int nextNode = game->layer[index].nextNode;
            game->layer[prevNode].nextNode = nextNode;
            game->layer[nextNode].prevNode = prevNode;
            game->layer[index].nodeType = 0;
            game->layer[index].prevNode = 0;
            game->layer[index].nextNode = 0;
        }
        ImGui::SameLine();
        if(layer.error)
        {
            ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Header, IM_COL32(200,50,50,255));
            ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_HeaderHovered, IM_COL32(255,50,50,255));
        }else{
            ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Header, IM_COL32(100,100,200,255));
            ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_HeaderHovered, IM_COL32(100,100,255,255));
        }
        if(ImGui::TreeNodeEx(layer.name, treeNodeflag))
        {
            if(ImGui::BeginCombo("Type", renderer::RenderStateString[layer.nodeType].c_str()))
            {
                for(auto index = 0; index < renderer::RenderStateCount; ++index){
                    if(ImGui::Selectable(renderer::RenderStateString[index].c_str(),(index == layer.nodeType) )){
                        layer.nodeType = index;
                        memcpy(&layer.name, &renderer::RenderStateString[index], 64);
                    }
                }
                ImGui::EndCombo();
            }
            if(layer.timeDuration > 10000000){
                ImGui::Text("Time: %08.2f mili", 9999999.00f);
            }else{
                ImGui::Text("Time: %08.2f mili", layer.timeDuration);
            }
            ImGui::LabelText("Vertex", "%s", layer.shaderVertex.c_str());
            ImGui::LabelText("Fragment", "%s", layer.shaderFragment.c_str());
            ImGui::SetNextItemWidth(150);
            if(ImGui::BeginCombo("BlendMode", renderer::blendmodes[layer.blendMode].c_str()))
            {
                for(auto index = 0; index < 8; ++index){
                    if(ImGui::Selectable(renderer::blendmodes[index].c_str(),(index == layer.blendMode) )){
                        layer.blendMode = index;
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::SetNextItemWidth(150);
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
            ImGui::SetCursorPosX(ImGui::GetWindowWidth()*0.5f-75);
            if(ImGui::Button("+", ImVec2(150,0.0f))){
                int nextNode = game->layer[index].nextNode;
                game->lastaddedLayer++;
                game->layer[game->lastaddedLayer] = renderer::RenderLayer{
                    .name = "",
                    .nextNode = nextNode,
                    .prevNode = index,
                    .nodeType = renderer::RenderState::null,
                };
                game->layer[nextNode].prevNode = game->lastaddedLayer;
                game->layer[index].nextNode = game->lastaddedLayer;
            }

            ImGui::TreePop();
        }
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::PopID();
        ImGui::EndGroup();
    }
    ImGui::End();

    ImGui::Render();
    ImGui_ImplRaylib_RenderDrawData(ImGui::GetDrawData());
}

void imguiRenderer::Close() {
}


void imguiRenderer::ImageCenter(ImVec2 viewportSize, int& width, int& height){
    float windowW = viewportSize.x;
    float windowH = viewportSize.x;
    float aspectRation = width/height;
    float imageScale = std::min(
        windowW / width,
        windowH / height
    );

    width *= imageScale;
    height*= imageScale;

    ImGui::SetCursorPos(ImVec2(
        (viewportSize.x - width) * 0.5f + ImGui::GetCursorPos().x, 
        (viewportSize.y - height)* 0.5f + ImGui::GetCursorPos().y)
    );
}
