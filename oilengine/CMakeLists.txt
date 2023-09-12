cmake_minimum_required(VERSION 3.27.0)

project(oilengine VERSION 1.0)

configure_file(OilengineConfig.h.in OilengineConfig.h)

set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(vendor/glfw)
add_subdirectory(vendor/GLAD)
add_subdirectory(vendor/imgui)


include_directories(${PROJECT_BINARY_DIR})

set(INCLUDE_FILE include/oil.h)
set(${INCLUDE_FILE} HEADER_FILE_ONLY ON)

add_library(oilengine SHARED 
                ${INCLUDE_FILE}
                src/oil/Application.cpp
                src/oil/Log.cpp
                src/oil/Layer.cpp
                src/oil/LayerStack.cpp
                src/Platform/Windows/WindowsWindow.cpp
                src/oil/imGui/imGuiLayer.cpp
                src/Platform/OpenGL/imGuiOpenGLRenderer.cpp
)

              


set(oilengine ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
include(GenerateExportHeader)
generate_export_header(oilengine
                     BASE_NAME oilengine
                     EXPORT_MACRO_NAME OIL_API
                     STATIC_DEFINE oil_BUILT_AS_STATIC
                        
)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)


target_compile_definitions(oilengine PRIVATE -DOIL_PLATFORM_WINDOWS -DOIL_BUILD_DLL -DGLFW_INCLUDE_NONE)


target_precompile_headers(oilengine PUBLIC src/pch/oilpch.h)

target_include_directories(oilengine PRIVATE
        src
        vendor/spdlog/include
        vendor/glfw/include
        vendor/GLAD/include
        vendor/imgui
)

target_link_directories(${PROJECT_NAME} 
                                PRIVATE vendor/glfw/build/src
                                PRIVATE vendor/
) 

target_link_libraries(${PROJECT_NAME} glfw3 glad imgui opengl32)

add_custom_command(TARGET oilengine POST_BUILD 
  COMMAND "${CMAKE_COMMAND}" -E copy 
     "$<TARGET_FILE:oilengine>"
     "../../application/build/Debug/$<TARGET_FILE_NAME:oilengine>" 
  COMMENT "Copying to output directory")