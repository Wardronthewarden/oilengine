cmake_minimum_required(VERSION 3.27.0)

project(barrel VERSION 1.0)

#configure_file(OilengineConfig.h.in OilengineConfig.h)

set(ENGINE_DIR ${CMAKE_SOURCE_DIR}/../oilengine)

set(PROJECT_LINK_LIBS ${ENGINE_DIR}/build/Debug/oilengine.lib)

link_directories(${ENGINE_DIR}/build/Debug)
include_directories(${ENGINE_DIR}/include
                    ${ENGINE_DIR}/build 
                    ${ENGINE_DIR}/src
                    ${ENGINE_DIR}/vendor/spdlog/include

)

add_executable(barrel src/ClientApp.cpp)
target_link_libraries(barrel PUBLIC ${PROJECT_LINK_LIBS})
target_compile_definitions(barrel PRIVATE -DOIL_PLATFORM_WINDOWS)

