#pragma once

#include <map>
#include <string>
#include <string_view>
#include <vector>

#include <raylib.h>

enum CustomerNeed_Main {
    Main_None = 0,
    Espresso,
    Late,
    Long,
};
std::map<CustomerNeed_Main, std::string_view> CustomerNeed_MainMap = {
    {CustomerNeed_Main::Espresso, "Espresso"},
    {CustomerNeed_Main::Late, "Late"},
    {CustomerNeed_Main::Long, "Long"},
    {CustomerNeed_Main::Main_None, "None"}
};
std::string_view Enum_to_string(CustomerNeed_Main* m)
{
    return CustomerNeed_MainMap[*m];
}
enum CustomerNeed_Sup {
    Sup_None = 0,
    Cow,
    Oat,
};
std::map<CustomerNeed_Sup, std::string_view> CustomerNeed_SupMap = {
    {CustomerNeed_Sup::Sup_None, "None"},
    {CustomerNeed_Sup::Oat, "Oat Milk"},
    {CustomerNeed_Sup::Cow, "Cow Milk"}
};
std::string_view Enum_to_string(CustomerNeed_Sup* m)
{
    return CustomerNeed_SupMap[*m];
}

class Customer {
  public:
    Customer() {
        active = false;
        clicked = false;
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
    bool clicked;
    Mesh* mesh;

    std::string name;
    CustomerNeed_Main need_main;
    int need_main_Count;
    CustomerNeed_Sup need_sup;
    int need_sup_Count;
    bool served;
    float stressBringer;
};

