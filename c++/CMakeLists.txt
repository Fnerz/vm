cmake_minimum_required(VERSION 3.16)

project(VM LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
# set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../pythonIntegrationTesting/src)
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

include_directories(${CMAKE_SOURCE_DIR}/src)

set(SDL3_DIR "${CMAKE_SOURCE_DIR}/src/SDL3-3.2.16/cmake")
find_package(SDL3 REQUIRED CONFIG PATHS ${SDL3_DIR})

set(VM_SOURCES
    src/main.cpp
    src/vm/vm.cpp
    src/vm/lexer.cpp
    src/vm/parser.cpp
    src/vm/types.cpp
    src/vm/lowerer.cpp
    src/vm/assembler.cpp
)
add_executable(VM ${VM_SOURCES})
target_link_libraries(VM PRIVATE SDL3::SDL3)

