#pragma once
#include "renderer.cpp"
#include "../game.cpp"

#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

class uiRenderer : public renderer {
  public:
    uiRenderer();
    ~uiRenderer() override;

    void Init() override;
    void Loop() override;
    void Close() override;

    Game* game;
    RenderTexture2D screenTexture;

  private:
    void DrawButton(Rectangle& buttonRect, std::string label, float duration, std::function<void()> function){
        if (GuiButton(buttonRect, label.c_str())) {
            if (game->playerActionStarted == false) {
                game->playerActionDuration = duration;
                game->playerAction.push(function);
                game->playerActionStarted = true;
            }
        }
    };
};

uiRenderer::uiRenderer() {}

uiRenderer::~uiRenderer() {}

void uiRenderer::Init() {
    // default materialValue
}

void uiRenderer::Loop() {
    DrawRectangle(screenTexture.texture.width - 50, (screenTexture.texture.height - 200.0f) / 2, 30, 200, BLUE);
    DrawRectangle(screenTexture.texture.width - 50 + 4, (screenTexture.texture.height - 200.0f) / 2 + 4, 30 - 8,
                  200 - 8, DARKBLUE);
    std::time_t currentClock = std::time(nullptr);
    char timebuffer[6];
    std::strftime(timebuffer, sizeof(timebuffer), "%H:%M", std::localtime(&currentClock));

    Rectangle buttonRect{30, 100, 150*GetWindowScaleDPI().y, 30*GetWindowScaleDPI().y};
    Rectangle labelRect{180*GetWindowScaleDPI().x, 100, 150*GetWindowScaleDPI().y, 30*GetWindowScaleDPI().y};

    DrawButton(buttonRect, "get Beans", 1.0f, [this]() { game->increaseBeanCount(); });
    GuiLabel(labelRect, std::string("Beans: ").append(std::to_string(game->beansAmount)).c_str());

    buttonRect.y += 40;
    labelRect.y += 40;
    DrawButton(buttonRect, "grind beans", 1.0f, [this]() { game->grindBeans(); });
    GuiLabel(labelRect, std::string("Grind. beans: ").append(std::to_string(game->grindedBeansAmount)).c_str());

    buttonRect.y += 40;
    labelRect.y += 40;
    DrawButton(buttonRect, "Small Cups", 2.0f, [this]() {game->increaseSmallCups();});
    GuiLabel(labelRect, std::string("Small Cups: ").append(std::to_string(game->smallCupsAmount)).c_str());

    buttonRect.y += 40;
    labelRect.y += 40;
    DrawButton(buttonRect, "Large Cups", 2.0f, [this]() {game->increaseBigCups();});
    GuiLabel(labelRect, std::string("Large Cups: ").append(std::to_string(game->bigCupsAmount)).c_str());

    buttonRect.y += 40;
    labelRect.y += 40;
    DrawButton(buttonRect, "Cow Milk", 2.0f, [this]() {game->increaseCowMilkCount();});
    GuiLabel(labelRect, std::string("Cow Milk: ").append(std::to_string(game->cowMilkAmount)).c_str());

    buttonRect.y += 40;
    labelRect.y += 40; 
    DrawButton(buttonRect, "oat Milk", 2.0f, [this]() { game->increaseOatMilkCount(); });
    GuiLabel(labelRect, std::string("Oat Milk: ").append(std::to_string(game->oatMilkAmount)).c_str());

    buttonRect.y += 40;
    labelRect.y += 40;
    DrawButton(buttonRect, "Espresso", 32.0f, [this]() { game->increaseEspressoAmount(); });
    GuiLabel(labelRect, std::string("Espresso: ").append(std::to_string(game->madeEspressoAmount)).c_str());

    buttonRect.y += 40;
    labelRect.y += 40;
    DrawButton(buttonRect, "Latte", 40.0f, [this]() { game->increaseLatteAmount(); });
    GuiLabel(labelRect, std::string("Late: ").append(std::to_string(game->madeLatteAmount)).c_str());

    buttonRect.y += 40;
    labelRect.y += 40;
    DrawButton(buttonRect, "LongCafe", 50.0f, [this]() { game->increaseLongCafeAmount(); });
    GuiLabel(labelRect, std::string("Long Cafe: ").append(std::to_string(game->madeLongCafeAmount)).c_str());

    buttonRect.y += 60;
    labelRect.y += 60;
    if (GuiButton(buttonRect, "serve")) {
        if (game->selectedCustomer != nullptr) {
            game->serveCustomer(game->selectedCustomer);
            if (game->checkCustomer(game->selectedCustomer)) {
                game->selectedCustomer->served = true;
                game->selectedCustomer->active = false;
                game->selectedCustomer->clicked = false;
                game->customersCount--;
            }
        }
    }
    GuiLabel(labelRect, game->GetFreeCustomer() != nullptr ? game->GetFreeCustomer()->name.c_str() : "");

    if (game->playerActionStarted) {
        game->playerActionValue += GetFrameTime();
        float actionBarWidth = 500.0f;
        float actionBarHeight = 50.0f;
        float actionBarStartValue = 0;
        float actionBarEndValue = game->playerActionDuration;
        GuiProgressBar(Rectangle{(screenTexture.texture.width - actionBarWidth) / 2,
                                 screenTexture.texture.height - 50.0f, actionBarWidth, actionBarHeight},
                       "", std::to_string(game->playerActionDuration).c_str(), &game->playerActionValue,
                       actionBarStartValue, actionBarEndValue);
        if (game->playerActionValue == actionBarEndValue) {
            if (!game->playerAction.empty()) {
                std::function<void()> action = game->playerAction.front();
                action();
                game->playerAction.pop();
                game->playerActionValue = 0;
                if (game->playerAction.empty()) {
                    game->playerActionStarted = false;
                }
            }
        }
    }

    DrawText(timebuffer, 5, 5, 48, BLACK);
    DrawText(std::to_string((int)(1.0f / GetFrameTime())).c_str(), screenTexture.texture.width - 100, 5, 48, BLACK);
}

void uiRenderer::Close() {}
