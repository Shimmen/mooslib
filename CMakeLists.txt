cmake_minimum_required(VERSION 3.1)
project(mooslib)

option(MOOSLIB_BuildTests "Build the mooslib test programs" ON)

add_library(mooslib INTERFACE)
target_include_directories(mooslib INTERFACE include/)

if (MOOSLIB_BuildTests)
    add_subdirectory(tests)
endif ()
