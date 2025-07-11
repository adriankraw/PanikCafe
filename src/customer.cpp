#pragma once

#include <string>
#include <vector>

enum CustomerNeed_Main {
    Main_None = 0,
    Espresso,
    Late,
    Long,
};
enum CustomerNeed_Sup {
    Sup_None = 0,
    Cow,
    Oat,
};

class Customer {
  public:
    Customer() {
        active = false;
        name = "customer";
        need_main = CustomerNeed_Main::Espresso;
        need_main_Count = 1;
        need_sup_Count = CustomerNeed_Sup::Sup_None;
        need_sup_Count = 0;
        served = 0;
        stressBringer = 1;
    };
    Customer(Customer &&) = default;
    Customer(const Customer &) = default;
    Customer &operator=(Customer &&) = default;
    Customer &operator=(const Customer &) = default;
    ~Customer() {};

    Customer* GetFreeCustomer(std::vector<Customer> *_customer);

    bool active;

    std::string name;
    CustomerNeed_Main need_main;
    int need_main_Count;
    CustomerNeed_Sup need_sup;
    int need_sup_Count;
    bool served;
    float stressBringer;
};

