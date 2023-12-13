#pragma once

#include "pch/oilpch.h"

#include "oil/core/core.h"
#include "oil/events/Event.h"

namespace oil{

    struct WindowProps{
        std::string Title;
        uint32_t Width;
        uint32_t Height;

        WindowProps(const std::string& title = "Oil Engine",
                    uint32_t width = 1600,
                    uint32_t height = 900)
                    :Title(title), Width(width), Height(height){}

    };


    // desktop window system interface
    class OIL_API Window{
    public:
        using EventCallbackFn = std::function<void(Event&)>;

        virtual ~Window(){}

        virtual void OnUpdate() = 0;

        virtual unsigned int GetWidth() const = 0;
        virtual unsigned int GetHeight() const = 0;

        // Window attributes
        virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
        virtual void SetVSync(bool enabled) = 0;
        virtual bool IsVSync() const = 0;

        virtual void* GetNativeWindow() const = 0;

        static Window* Create(const WindowProps& props = WindowProps());
        

    };
}