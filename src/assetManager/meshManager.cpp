#pragma once
#include "assetManager.cpp"
#include <raylib.h>

class meshManager : public assetManager<Model>{
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
    list.push_back(LoadModel(path.c_str()));
}

void meshManager::Export(){
}
