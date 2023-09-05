#include "Application.h"

#include "oil/events/ApplicationEvent.h"
#include "oil/Log.h"

namespace oil{
oil::Application::Application()
{

}

Application::~Application()
{
}
void Application::Run()
{
    WindowResizeEvent e(1280, 720);
    OIL_TRACE(e);

    while (true){
        
    };
}
}
