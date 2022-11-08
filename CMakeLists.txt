cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)

project(RESOSpec)

set(SPECTER_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(SPECTER_LIBRARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Specter/lib)

add_subdirectory(Specter)
add_subdirectory(SpecProject)