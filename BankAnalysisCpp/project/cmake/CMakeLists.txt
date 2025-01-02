cmake_minimum_required(VERSION 3.28.3)

set(PROJECT_NAME backenTest)

Project(${PROJECT_NAME} LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message("Compiler: ${CMAKE_CXX_COMPILER_ID} Version: ${CMAKE_CXX_COMPILER_VERSION}")
add_subdirectory(backendTest)
