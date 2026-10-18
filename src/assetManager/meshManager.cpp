#pragma once
#include "assetManager.cpp"
#include "../gameObject.cpp"

#include <raylib.h>

class meshManager : public assetManager<GameObject>{
public:
    meshManager();
    meshManager(const meshManager&) = default;
    ~meshManager() override;

    void Import(std::string) override;
    void Export() override;

private:
    
};

meshManager::meshManager() {
}

meshManager::~meshManager() {
}

void meshManager::Import(std::string path){
    Model model = LoadModel(path.c_str());
    GameObject gameObject = GameObject(path.c_str(), model, Vector3{0,0,0}, Vector3{0,0,0}, Vector3{1,1,1});
    list.push_back(gameObject);
}

void meshManager::Export(){
}
