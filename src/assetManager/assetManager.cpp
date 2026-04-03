#pragma once

#include <vector>
#include <string>

template<typename T>
class assetManager {
public:
    virtual ~assetManager() = default;

    virtual void Import(std::string){};
    virtual void Export(){};

    std::vector<T> list;

private:

};
