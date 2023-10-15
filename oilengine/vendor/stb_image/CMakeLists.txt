cmake_minimum_required(VERSION 3.27.0)

project(stb_img)

add_library(stb_img stb_image.cpp)

target_include_directories(glad PRIVATE
    include/
)