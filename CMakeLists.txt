add_subdirectory(oilengine/ oilengine/build/)

cmake_minimum_required(VERSION 3.27.0)

project(barrel VERSION 1.0)

#configure_file(OilengineConfig.h.in OilengineConfig.h)

set(ENGINE_DIR ${CMAKE_SOURCE_DIR}/oilengine)

include_directories(${ENGINE_DIR}/include
                    ${ENGINE_DIR}/build 
                    ${ENGINE_DIR}/src
                    ${ENGINE_DIR}/vendor/spdlog/include
                    ${ENGINE_DIR}/vendor/glfw/include
                    ${ENGINE_DIR}/vendor/glm

)

add_executable(barrel application/src/ClientApp.cpp)
target_link_directories(barrel PRIVATE ${ENGINE_DIR}/build/Debug)
target_link_libraries(barrel PUBLIC oilengine)
target_compile_definitions(barrel PRIVATE -DOIL_PLATFORM_WINDOWS)