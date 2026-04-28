cmake_minimum_required(VERSION 3.16)

project(VM LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../pythonIntegrationTesting/src)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

include_directories(${CMAKE_SOURCE_DIR}/src)

find_package(pybind11 REQUIRED)
pybind11_add_module(VM
    src/main.cpp
    src/vm/vm.cpp
    src/vm/lexer.cpp
    src/vm/parser.cpp
    src/vm/types.cpp

)
