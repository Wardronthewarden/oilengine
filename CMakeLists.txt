add_subdirectory(oilengine/)


cmake_minimum_required(VERSION 3.27.0)

project(Client VERSION 1.0)

project(Editor VERSION 1.0)
#configure_file(OilengineConfig.h.in OilengineConfig.h)

set(ENGINE_DIR ${CMAKE_SOURCE_DIR}/oilengine)

include_directories(${ENGINE_DIR}/include
                    ${ENGINE_DIR}/build 
                    ${ENGINE_DIR}/src
                    ${ENGINE_DIR}/vendor/spdlog/include
                    ${ENGINE_DIR}/vendor/glfw/include
                    ${ENGINE_DIR}/vendor/glm
                    ${ENGINE_DIR}/vendor/EnTT/include
                    ${ENGINE_DIR}/vendor/yaml-cpp/include
                    ${ENGINE_DIR}/vendor/ImGuizmo

)

#Client

add_executable(Client application/src/ClientApp.cpp
                        application/src/Client2D.cpp
                        application/src/ParticleSystem.cpp
)


target_link_libraries(Client PUBLIC oilengine)
target_compile_definitions(Client PRIVATE -DOIL_DEBUG)

set_property(TARGET Client PROPERTY CXX_STANDARD 23)

# Editor

add_executable(Editor   editor/src/EditorApp.cpp
                        editor/src/EditorLayer.cpp
                        editor/src/Panels/SceneHierarchyPanel.cpp
)

target_include_directories(Editor PUBLIC editor/src)

set_property(TARGET Editor PROPERTY CXX_STANDARD 23)
target_link_libraries(Editor PUBLIC oilengine)
target_compile_definitions(Editor PRIVATE -DOIL_DEBUG)

set_property(TARGET Editor PROPERTY CXX_STANDARD 23)