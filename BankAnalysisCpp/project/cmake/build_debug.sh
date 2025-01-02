#!/bin/bash

mkdir -p ../../build/cmake/
mkdir -p ../../bin/cmake/
cd ../../build/cmake
#cmake -DCMAKE_PREFIX_PATH="/usr/lib/qt6/bin;/usr/lib/x86_64-linux-gnu/cmake" ../../project/cmake -DCMAKE_CONFIGURATION_TYPES=Debug
cmake -DCMAKE_PREFIX_PATH="/opt/Qt/6.8.1/gcc_64" ../../project/cmake -DCMAKE_CONFIGURATION_TYPES=Debug
cmake --build . --parallel 4

cp ../../bin/cmake/backendCpp/Debug/backendCpp ../../run/
