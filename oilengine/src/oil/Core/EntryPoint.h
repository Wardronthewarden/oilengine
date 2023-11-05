#pragma once

#ifdef OIL_PLATFORM_WINDOWS

extern oil::Application* oil::CreateApplication();

int main(int argc, char** argv){
   
    oil::Log::Init();
    OIL_CORE_WARN("Init log!");
    auto app = oil::CreateApplication();
    app->Run();
    delete app;
  
}

#endif