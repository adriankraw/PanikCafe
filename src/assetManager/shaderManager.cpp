
#include "assetManager.cpp"

#include <raylib.h>

class shaderManager : public assetManager<Shader>{
public:
    shaderManager();
    ~shaderManager() override;

    void Import(std::string) override;
    void Export() override;

private:
    
};

shaderManager::shaderManager() {
}

shaderManager::~shaderManager() {
}

void shaderManager::Import(std::string path){
    int index = path.find(';');
    std::string first = path.substr(0,index);
    std::string second = path.substr((index+1)>path.size()?index:index+1, path.length());
    list.push_back(LoadShader(
        first==""?0:TextFormat(first.c_str(), 330),
        second==""?0:TextFormat(second.c_str(), 330)));
}

void shaderManager::Export(){
}
