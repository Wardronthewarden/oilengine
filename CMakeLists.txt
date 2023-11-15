add_subdirectory(oilengine/)


cmake_minimum_required(VERSION 3.27.0)

project(Client VERSION 1.0)

#configure_file(OilengineConfig.h.in OilengineConfig.h)

set(ENGINE_DIR ${CMAKE_SOURCE_DIR}/oilengine)

include_directories(${ENGINE_DIR}/include
                    ${ENGINE_DIR}/build 
                    ${ENGINE_DIR}/src
                    ${ENGINE_DIR}/vendor/spdlog/include
                    ${ENGINE_DIR}/vendor/glfw/include
                    ${ENGINE_DIR}/vendor/glm

)


add_executable(Client application/src/ClientApp.cpp
                        application/src/Client2D.cpp)

target_link_libraries(Client PUBLIC oilengine)
target_compile_definitions(Client PRIVATE -DOIL_DEBUG)

set_property(TARGET Client PROPERTY CXX_STANDARD 23)