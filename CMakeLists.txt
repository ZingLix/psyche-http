﻿# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("psyche-http")
add_definitions(-std=c++17)
# Include sub-projects.
add_subdirectory ("psyche-http")
add_subdirectory ("example")