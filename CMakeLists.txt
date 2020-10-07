cmake_minimum_required(VERSION 3.1)
project(mooslib)

add_library(mooslib INTERFACE)
target_include_directories(mooslib INTERFACE include)

# Build tests if this is invoked directly from CMake (i.e., not through add_subdirectory)
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
    add_subdirectory(tests)
endif ()
