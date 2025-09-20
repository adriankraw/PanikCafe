#pragma once
#include <map>
#include <raylib.h>

class renderer {
public:
    virtual ~renderer() = default;

    virtual void Init(){};
    virtual void Loop(){};
    virtual void Close(){};

    enum RenderState{
        light,
        shadow,
        screen,
    };

    std::map<RenderState,Shader> shader{};
    std::map<RenderState,RenderTexture2D> renderTextureIn{};
    std::map<RenderState,RenderTexture2D> renderTextureOut{};

private:
    
};
