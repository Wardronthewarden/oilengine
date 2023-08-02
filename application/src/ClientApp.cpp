#include <oil.h>

class Barrel : public oil::Application{
public:
    Barrel() {};
    ~Barrel() {};



};


oil::Application* oil::CreateApplication(){
    return new Barrel();
}