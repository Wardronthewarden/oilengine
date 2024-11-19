#include "OpenGLValidation.h"
#include "oil/core/Log.h"

namespace oil{

    void getOpenGLError(const char* operationName)
    {
        const char* error;
        switch (glGetError()){
            case GL_INVALID_ENUM:                  error = "GL error: INVALID_ENUM";
            case GL_INVALID_VALUE:                 error = "GL error: INVALID_VALUE";
            case GL_INVALID_OPERATION:             error = "GL error: INVALID_OPERATION";
            case GL_STACK_OVERFLOW:                error = "GL error: STACK_OVERFLOW";
            case GL_STACK_UNDERFLOW:               error = "GL error: STACK_UNDERFLOW";
            case GL_OUT_OF_MEMORY:                 error = "GL error: OUT_OF_MEMORY";
            case GL_INVALID_FRAMEBUFFER_OPERATION: error = "GL error: INVALID_FRAMEBUFFER_OPERATION";
            case GL_NO_ERROR:                      return;
            default:                               return;
        }
        OIL_CORE_ERROR("OpenGL encountered a problem on operation: {0}", operationName);
        OIL_CORE_ERROR("\tGL error: {0}", error);
        __debugbreak();
    }
}