#pragma once

#include <functional>
#include <queue>
#include <string>

#include "customer.cpp"
#include "gameObject.cpp"
#include <raylib.h>

class Game {
  public:
    Game();
    Game(Game &&) = default;
    Game(const Game &) = default;
    Game &operator=(Game &&) = default;
    Game &operator=(const Game &) = default;
    ~Game();

    Customer *GetFreeCustomer();
    void increaseBeanCount();
    void grindBeans();
    void increaseSmallCups();
    void increaseBigCups();
    void increaseCowMilkCount();
    void increaseOatMilkCount();
    void increaseEspressoAmount();
    void increaseLatteAmount();
    void increaseLongCafeAmount();

    std::string playerName;
    int playerAge;
    float playerActionValue;
    bool playerActionStarted;
    Model playerModel;
    std::queue<std::function<void()>> playerAction;

    std::vector<GameObject> models;

    std::vector<Customer> customers;
    int customersCount;
    int customersMax;
    float stressLevel;
    float customersCooldown;
    float customersCooldownMax;
    int smallCupsAmount;
    int bigCupsAmount;
    int beansAmount;
    int grindedBeansAmount;
    int cowMilkAmount;
    int oatMilkAmount;
    int madeEspressoAmount;
    int madeLatteAmount;
    int madeLongCafeAmount;
};

Game::Game() {}

Game::~Game() {}

Customer *Game::GetFreeCustomer() {
    for (size_t c = 0; c < customers.size(); ++c) {
        if (customers.at(c).active == false) {
            return &(customers.at(c));
            break;
        }
    }
    return nullptr;
}
void Game::increaseBeanCount() { beansAmount += 30; }
void Game::grindBeans() {
    grindedBeansAmount += 30;
    beansAmount -= 30;
}
void Game::increaseSmallCups() { smallCupsAmount++; }
void Game::increaseBigCups() { bigCupsAmount++; }
void Game::increaseCowMilkCount() { cowMilkAmount++; }
void Game::increaseOatMilkCount() { oatMilkAmount++; }
void Game::increaseEspressoAmount() {
    madeEspressoAmount++;
    grindedBeansAmount -= 30;
}
void Game::increaseLatteAmount() {
    madeLatteAmount++;
    grindedBeansAmount -= 30;
    oatMilkAmount -= 0.25f;
}
void Game::increaseLongCafeAmount() {
    madeLongCafeAmount++;
    grindedBeansAmount -= 30;
}
