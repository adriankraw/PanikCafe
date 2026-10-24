#pragma once

#include <memory>
#include <string>

#include <raylib.h>

Vector2 GetGameMousePosition();

Vector2 mouseOffset = Vector2{0,0};
Vector2 mouseScale  = Vector2{1,1};

Vector2 GetGameMousePosition()
{
    Vector2 mouse = GetMousePosition();

    Vector2 result = {
        (mouse.x - mouseOffset.x) / mouseScale.x,
        (mouse.y - mouseOffset.y) / mouseScale.y
    };
    return result;
}

#define GUI_POINTER_POSITION    GetGameMousePosition()



class config {
public:
    static config& getInstance(){
        static config instance;

        return instance;
    }
    std::string fontNormalPath = "./Assets/fonts/raleway-latin-500-normal.ttf";
    Font fontNormal;

private:
    config(){};

public:

    config(config const&) = delete;
    void operator=(config const&) = delete;

    void LoadConfig(){
        getInstance().fontNormal = LoadFont(fontNormalPath.c_str());
        if (!IsWindowState(FLAG_WINDOW_HIGHDPI))
        {
            getInstance().fontNormal.baseSize = 16 * GetWindowScaleDPI().y;
        }else{
            getInstance().fontNormal.baseSize = 16;
        }
    }
};
