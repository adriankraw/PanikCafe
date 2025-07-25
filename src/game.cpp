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
    void serveCustomer(Customer *);
    bool checkCustomer(Customer *);
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
    float playerActionDuration; //will need a getter. Dont change the duraction if action is running;
    bool playerActionStarted;
    Model* playerModel = nullptr;
    std::queue<std::function<void()>> playerAction;

    std::vector<GameObject> models;

    std::vector<Customer> customers;
    Customer *selectedCustomer;
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

Game::Game() {
    playerName = "olantern";
    playerAge = 28;
    playerActionValue = 0.0f;
    playerActionDuration = 1.0f;
    playerActionStarted = false;
    models = std::vector<GameObject>();
    customers = std::vector<Customer>(10);
    customersCount = 0;
    customersMax = 10;
    customersCooldown = 5;
    customersCooldownMax = 5;
    smallCupsAmount = 10;
    bigCupsAmount = 10;
    beansAmount = 30;
    grindedBeansAmount = 0;
    cowMilkAmount = 0;
    oatMilkAmount = 0;
    madeEspressoAmount = 20;
    madeLatteAmount = 0;
    madeLongCafeAmount = 0;
}

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
void Game::serveCustomer(Customer *cu) {
    switch (cu->need_main) {
    case CustomerNeed_Main::Espresso: {
        while (cu->need_main_Count > 0 && madeEspressoAmount > 0) {
            cu->need_main_Count--;
            madeEspressoAmount--;
        }
    } break;
    case CustomerNeed_Main::Late: {
        while (cu->need_main_Count > 0 && madeLatteAmount > 0) {
            cu->need_main_Count--;
            madeLatteAmount--;
        }
    } break;
    case CustomerNeed_Main::Long: {
        while (cu->need_main_Count > 0 && madeLongCafeAmount > 0) {
            cu->need_main_Count--;
            madeLongCafeAmount--;
        }
    } break;
    case CustomerNeed_Main::Main_None: {
    } break;
    }
    switch (cu->need_sup) {
    case CustomerNeed_Sup::Cow: {
        while (cu->need_sup_Count > 0 && cowMilkAmount > 0) {
            cu->need_sup_Count--;
            cowMilkAmount--;
        }
    } break;
    case CustomerNeed_Sup::Oat: {
        while (cu->need_sup_Count > 0 && oatMilkAmount > 0) {
            cu->need_sup_Count--;
            oatMilkAmount--;
        }
    } break;
    case CustomerNeed_Sup::Sup_None: {
    } break;
    }
}
bool Game::checkCustomer(Customer *cu) {
    if (cu->need_main_Count == 0 && cu->need_sup_Count == 0) {
        return true;
    }
    return false;
}
