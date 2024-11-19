#include <oil/core/core.h>

#include <GLAD/glad.h>

namespace oil{

    void getOpenGLError(const char* operationName);

    #ifdef OIL_DEBUG

        #define GL_VALIDATE(x) {getOpenGLError(x);}

    #else

        #define GL_VALIDATE(x)

    #endif

}