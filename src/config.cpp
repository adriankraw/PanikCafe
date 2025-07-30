#include <memory>
#include <raylib.h>

class config {
public:
    static config& getInstance(){
        static config instance;

        return instance;
    }

    Font fontNormal;

private:
    config(){};

public:

    config(config const&) = delete;
    void operator=(config const&) = delete;

    void LoadConfig(){
        getInstance().fontNormal = LoadFont("./fonts/raleway-latin-500-normal.ttf");
    }
};
