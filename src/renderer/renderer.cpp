#pragma once
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
        light,
        shadow,
        pbr,
        screen,
    };

    struct RenderLayer{
        char name[64];
        bool clearBackground;
        int cullMode;
        int blendMode;
        int backgroundColor[4] {0,0,0,0};
        Shader shader;
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
