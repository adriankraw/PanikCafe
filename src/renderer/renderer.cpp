#pragma once
#include "imgui.h"
#include <cstddef>
#include <vector>

#include <raylib.h>
#include <rlgl.h>


class renderer {
public:
    virtual ~renderer() = default;

    virtual void Init(){};
    virtual void Loop(){};
    virtual void Close(){};

    enum RenderState{
        null = 0,
        cull,
        cullEnd,
        blend,
        blendEnd,
        depth,
        light,
        shadow,
        shadowToScreen,
        pbr,
        screen,
    };
    static const int RenderStateCount = 11;
    std::string RenderStateString[RenderStateCount] = {
        "null",
        "cull",
        "cullEnd",
        "blend",
        "blendEnd",
        "depth",
        "light",
        "shadow",
        "shadowToScreen",
        "pbr",
        "screen",
    };

    struct RenderLayer{
        bool error;
        char name[64];
        int nextNode = 0;
        int prevNode = 0;
        int nodeType = RenderState::null;
        bool clearBackground;
        float calctimeDuration = 0;
        double timeDuration = 0;
        int cullMode;
        int blendMode;
        int backgroundColor[4] {0,0,0,0};
        bool reloadShader = false;
        Shader shader;
        int inputTexturesCount; //contains how many input Textures this node would need
        int outpitTexturesCount; //contains how many output Textures this node would need
        std::string shaderVertex;
        long shaderVertexModTime;
        std::string shaderFragment;
        long shaderFragmentModTime;
        std::vector<RenderTexture2D> inputTexture{};
        std::vector<RenderTexture2D> outputTexture{};
    };

    std::string blendmodes[8] = {
        "ALPHA",
        "ADDITIVE",
        "MULTIPLIED",
        "ADD_COLORS",
        "SUBTRACT_COLORS",
        "ALPHA_PREMULTIPLY",
        "CUSTOM",
        "CUSTOM_SEPARATE"
    };
    std::string cullmodes[2] = {
        "FRONT",
        "BACK"
    };

private:
    
};
