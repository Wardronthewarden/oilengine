#include <oil.h>

#include "oilengine_export.h"

class Barrel : public oil::Application{
public:
    Barrel() {};
    ~Barrel() {};



};


oil::Application* oil::CreateApplication(){
    OIL_INFO("Initialized client app!");
    return new Barrel();
}