cmake_minimum_required(VERSION 3.27.0)

project(oilengine VERSION 1.0)

configure_file(OilengineConfig.h.in OilengineConfig.h)

add_subdirectory(vendor/glfw)
add_subdirectory(vendor/GLAD)
add_subdirectory(vendor/imgui)
add_subdirectory(vendor/stb_image)


include_directories(${PROJECT_BINARY_DIR})

add_library(oilengine STATIC 
                src/oil/core/Application.cpp
                src/oil/core/Log.cpp
                src/oil/core/Layer.cpp
                src/oil/core/LayerStack.cpp
                src/oil/imGui/imGuiBuild.cpp
                src/oil/imGui/imGuiLayer.cpp
                src/Platform/Windows/WindowsWindow.cpp
                src/Platform/Windows/WindowsInput.cpp
                src/Platform/OpenGL/OpenGLContext.cpp
                src/Platform/OpenGL/OpenGLBuffer.cpp
                src/Platform/OpenGL/OpenGLFrameBuffer.cpp
                src/Platform/OpenGL/OpenGLVertexArray.cpp
                src/Platform/OpenGL/OpenGLRenderAPI.cpp
                src/Platform/OpenGL/OpenGLShader.cpp
                src/Platform/OpenGL/OpenGLTexture.cpp
                src/oil/Scene/Scene.cpp
                src/oil/Renderer/Renderer.cpp
                src/oil/Renderer/Renderer2D.cpp
                src/oil/Renderer/Buffer.cpp
                src/oil/Renderer/FrameBuffer.cpp
                src/oil/Renderer/Shader.cpp
                src/oil/Renderer/Texture.cpp
                src/oil/Renderer/SubTexture2D.cpp
                src/oil/Renderer/VertexArray.cpp
                src/oil/Renderer/RenderAPI.cpp
                src/oil/Renderer/RenderCommand.cpp
                src/oil/Renderer/OrthographicCamera.cpp
                src/oil/Renderer/OrthographicCameraController.cpp

)

              
set_property(TARGET oilengine PROPERTY CXX_STANDARD 23)

set(oilengine ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
include(GenerateExportHeader)
generate_export_header(oilengine
                     BASE_NAME oilengine
                     EXPORT_MACRO_NAME OIL_API
                     STATIC_DEFINE oil_BUILT_AS_STATIC
                        
)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)


target_compile_definitions(oilengine PRIVATE -DOIL_DEBUG -DGLFW_INCLUDE_NONE)


target_precompile_headers(oilengine PUBLIC src/pch/oilpch.h)

target_include_directories(oilengine PUBLIC
        src
        vendor/spdlog/include
        vendor/glfw/include
        vendor/GLAD/include
        vendor/imgui
        vendor/stb_image/include
        vendor/glm
        vendor/EnTT/include
)

target_link_directories(${PROJECT_NAME} 
                                PUBLIC vendor/
                                PUBLIC vendor/glfw/src
) 

target_link_libraries(${PROJECT_NAME} PUBLIC glfw3 glad imgui opengl32 stb_img)
