#include "notification.cpp"
#include "raylib.h"

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
    DrawText(text.c_str(), posX+10, posY+25, 14, WHITE);
}
