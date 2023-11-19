#include "pch/oilpch.h"
#include "oil/core/Input.h"

#include "oil/core/Application.h"
#include <GLFW/glfw3.h>

namespace oil{

    bool Input::IsKeyPressed(int keycode)
    {
        auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());

        auto state = glfwGetKey(window, keycode);

        return state == GLFW_PRESS || state == GLFW_REPEAT;
    }
    bool Input::IsMouseButtonPressed(int button)
    {
        auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());

        auto state = glfwGetMouseButton(window, button);

        return state == GLFW_PRESS;
    }
    std::pair<float, float> Input::GetMousePos()
    {
        auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        return {(float)xpos, (float)ypos};
    }
    float Input::GetMouseX()
    {
        auto[x, y] = GetMousePos();

        return x;
    }
    float Input::GetMouseY()
    {
        auto[x, y] = GetMousePos();

        return y;
    }
}