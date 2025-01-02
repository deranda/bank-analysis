cmake_minimum_required(VERSION 3.28.3)

# Project Name
set(PROJECT_NAME backendCpp)
project(${PROJECT_NAME} LANGUAGES CXX)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(PALTFORM x64)
else()
    set(PLATFORM x86)
endif()
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../../bin/cmake/${PROJECT_NAME}/${CMAKE_CONFIGURATION_TYPES}/${PLATFORM})

# Find Qt
set(Qt6_DIR "/opt/Qt/6.8.1/gcc_64/lib/cmake/Qt6")
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network HttpServer)
message(STATUS "Using Qt version: ${Qt6_VERSION}")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(SRC_DIR ../../../src)

# Add your source files
set(SRC_FILES
    ${SRC_DIR}/main.cpp
    ${SRC_DIR}/WalletServer.cpp
    ${SRC_DIR}/Logger.cpp
)

set(INC_FILES
    ${SRC_DIR}/WalletServer.h
    ${SRC_DIR}/BackendDefs.h
    ${SRC_DIR}/Logger.h
)

# Add your executable
add_executable(${PROJECT_NAME} ${SRC_FILES} ${INC_FILES})

# Link Qt libraries
target_link_libraries(${PROJECT_NAME} 
    PRIVATE Qt6::Core
    PRIVATE Qt6::Gui 
    PRIVATE Qt6::Widgets
    PRIVATE Qt6::Network
    PRIVATE Qt6::HttpServer
)

# Include directories
target_include_directories(${PROJECT_NAME} 
   PRIVATE ${SRC_DIR}
)
