#include "pch/oilpch.h"
#include "OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <GLAD/glad.h>


namespace oil{
    OpenGLContext::OpenGLContext(GLFWwindow *windowHandle)
        : m_WindowHandle(windowHandle)
    {
        OIL_CORE_ASSERT(windowHandle, "Window handle is null!")
    }

    void OpenGLContext::Init()
    {
        glfwMakeContextCurrent(m_WindowHandle);
        int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        OIL_CORE_ASSERT(status, "Failed to initialize GLAD");

        OIL_CORE_INFO("OpenGL Info: ");
        OIL_CORE_INFO("  Vendor: {0}", (char*)glGetString(GL_VENDOR));
        OIL_CORE_INFO("  Renderer: {0}", (char*)glGetString(GL_RENDERER));
        OIL_CORE_INFO("  Version: {0}", (char*)glGetString(GL_VERSION));
    }
    void OpenGLContext::SwapBuffers()
    {
        glfwSwapBuffers(m_WindowHandle);
    }
}