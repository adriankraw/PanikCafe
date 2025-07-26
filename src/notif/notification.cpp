#pragma once
#include <string>

class notification {
public:
    virtual ~notification();
    virtual void Show(float posX, float poxY) = 0;
    virtual void AddAge(float _age);
    bool visible = false;
    std::string text;
    float width = 200;
    float height = 50;
    bool destroyMe = false;

private:
    float age = 0; 
};
notification::~notification(){
    age = 0;
    visible = false;
}
void notification::AddAge(float _age){
    age += _age;
    if(age >= 5.0f){
        visible = false;
    }
}
