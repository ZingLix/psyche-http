﻿# CMakeList.txt : CMake project for psyche-http, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
file(GLOB src "*.cpp")
add_executable (psyche-http ${src})
target_link_libraries (psyche-http psyche pthread)
# TODO: Add tests and install targets if needed.
