#!/bin/sh

mkdir build
cd build
cmake -G "MinGW Makefiles" ..
cmake --build .