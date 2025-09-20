#pragma once

#include <vector>

template<typename T>
class assetManager {
public:
    virtual ~assetManager() = default;

    virtual void Import(std::string){};
    virtual void Export(){};

    std::vector<T> list;

private:

};
