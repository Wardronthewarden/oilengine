#pragma once

#ifdef OIL_PLATFORM_WINDOWS

extern oil::Application* oil::CreateApplication();

int main(int argc, char** argv){
    
    auto app = oil::CreateApplication();
    app->Run();
    delete app;
    return 0;    
}

#endif