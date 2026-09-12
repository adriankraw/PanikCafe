#pragma once

#include <memory>
#include <string>

#include <raylib.h>

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
