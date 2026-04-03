#include "notification.cpp"
#include "../config.cpp"

class AlertNotification : public notification{
public:
    ~AlertNotification(){};
    AlertNotification(std::string);
    void Show(float posX, float posY);

private:

};

AlertNotification::AlertNotification(std::string s){
    text = s;
    visible = true;
}
void AlertNotification::Show(float posX, float posY){
    DrawRectangle(posX, posY, width, height, RED);
    DrawRectangle(posX+1, posY+1, width-2, height-2, BLACK);
    DrawTextEx(config::getInstance().fontNormal ,text.c_str(), Vector2{posX+10,posY+25}, 14, 0, WHITE);
}
