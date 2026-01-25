#pragma once
#include "raylib.h"
#include "rlgl.h"

#include <array>
#include <cstddef>
#include <functional>
#include <queue>
#include <string>

#include "customer.cpp"
#include "gameObject.cpp"
#include "scene.cpp"


class Game {
  public:
    Game();
    Game(Game &&) = default;
    Game(const Game &) = default;
    Game &operator=(Game &&) = default;
    Game &operator=(const Game &) = default;
    ~Game();

    Customer *GetFreeCustomer();
    void SetupCustomer(Customer *);
    void serveCustomer(Customer *);
    bool checkCustomer(Customer *);
    void GetCaffeeRecepe(int, int *, int *);
    void increaseBeanCount();
    void grindBeans();
    void increaseSmallCups();
    void increaseBigCups();
    void increaseCowMilkCount();
    void increaseOatMilkCount();
    void increaseEspressoAmount();
    void increaseLatteAmount();
    void increaseLongCafeAmount();
    void updateObjectTransformByAnim(Vector3 *trans, int model_index, int frame);

    std::vector<GameObject> meshes;
    std::vector<Scene> scenes;


    std::string playerName{"playerName"};
    int playerAge{27};
    float playerActionValue{0};
    float playerActionDuration{};
    bool playerActionStarted;
    Model *playerModel = nullptr;
    std::queue<std::function<void()>> playerAction;



    std::vector<Customer> customers{};
    Customer *selectedCustomer{};
    int customersCount{0};
    int customersMax{0};
    float stressLevel{0};
    float customersCooldown{0};
    float customersCooldownMax{0};
    int smallCupsAmount{0};
    int bigCupsAmount{0};
    int beansAmount{0};
    int grindedBeansAmount{0};
    int cowMilkAmount{0};
    int oatMilkAmount{0};
    int madeEspressoAmount{0};
    int madeLatteAmount{0};
    int madeLongCafeAmount{0};

  private:
    int spawnedCustomers = 0;
    std::array<std::array<int, 255>, 255> noiseMap = {{}};
    std::array<int, 255> lookUpTable = {};
};

Game::Game() {
    playerName = "olantern";
    playerAge = 28;
    playerActionValue = 0.0f;
    playerActionDuration = 1.0f;
    playerActionStarted = false;
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

    for (size_t x = 0; x < noiseMap.size(); ++x) {
        for (size_t y = 0; y < noiseMap[x].size(); ++y) {
            noiseMap[x][y] = GetRandomValue(0, 5);
        }
    }
    for (size_t x = 0; x < lookUpTable.size(); ++x) {
        lookUpTable[x] = GetRandomValue(0, 254);
    }
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
void Game::SetupCustomer(Customer *cust) {
    spawnedCustomers++;
    int x = lookUpTable[spawnedCustomers];
    int y = lookUpTable[(x + spawnedCustomers) % 255];

    int caffeeEnum = noiseMap[x][y];

    GetCaffeeRecepe(caffeeEnum, &x, &y);
    cust->need_main = (CustomerNeed_Main)x;
    cust->need_main_Count = 1;
    cust->need_sup = (CustomerNeed_Sup)y;
    cust->need_sup_Count = 1;
}
void Game::GetCaffeeRecepe(int rec, int *mainingredient, int *supIngredient) {
    // This will be changed in the future with a propper Recepe map
    switch (rec) {
    case 0:
        *mainingredient = (int)CustomerNeed_Main::Espresso;
        *supIngredient = (int)CustomerNeed_Sup::Sup_None;
        break;
    case 1:
        *mainingredient = (int)CustomerNeed_Main::Long;
        *supIngredient = (int)CustomerNeed_Sup::Sup_None;
        break;
    case 2:
        *mainingredient = (int)CustomerNeed_Main::Espresso;
        *supIngredient = (int)CustomerNeed_Sup::Cow;
        break;
    case 3:
        *mainingredient = (int)CustomerNeed_Main::Espresso;
        *supIngredient = (int)CustomerNeed_Sup::Oat;
        break;
    case 4:
        *mainingredient = (int)CustomerNeed_Main::Late;
        *supIngredient = (int)CustomerNeed_Sup::Cow;
        break;
    case 5:
        *mainingredient = (int)CustomerNeed_Main::Late;
        *supIngredient = (int)CustomerNeed_Sup::Oat;
        break;
    }
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


