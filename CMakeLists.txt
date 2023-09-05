cmake_minimum_required(VERSION 3.27.0)

project(oilengine VERSION 1.0)

configure_file(OilengineConfig.h.in OilengineConfig.h)

add_subdirectory(oilengine/ oilengine/build/)

add_subdirectory(application/ application/build/)


target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_BINARY_DIR})