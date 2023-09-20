#pragma once 

namespace oil{

    class OIL_API GraphicsContext{
    public:
        virtual void Init() = 0;
        virtual void SwapBuffers() = 0;
    };
}