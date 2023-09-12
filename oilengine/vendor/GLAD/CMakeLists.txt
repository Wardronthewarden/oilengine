cmake_minimum_required(VERSION 3.27.0)

project(glad)

add_library(glad src/glad.c)

target_include_directories(glad PRIVATE
    include/
)