#include "notification.cpp"
#include "raylib.h"
#include "../config.cpp"

class OrderNotification : public notification{
public:
    ~OrderNotification(){};
    OrderNotification(std::string);
    void Show(float posX, float posY);

private:

};

OrderNotification::OrderNotification(std::string s){
    text = s;
    visible = true;
}
void OrderNotification::Show(float posX, float posY){
    DrawRectangle(posX, posY, width, height, BROWN);
    DrawRectangle(posX+2, posY+2, width-4, height-4, BLACK);
    DrawTextEx(config::getInstance().fontNormal ,text.c_str(), Vector2{posX+10,posY+25}, 14, 0, WHITE);
}
