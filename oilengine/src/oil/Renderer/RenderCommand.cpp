#include "pch/oilpch.h"
#include "RenderCommand.h"

#include "Platform/OpenGL/OpenGLRenderAPI.h"

namespace oil{
    RenderAPI* RenderCommand::s_RenderAPI = new OpenGLRenderAPI;
}