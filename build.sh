#!/bin/sh

cd build
cmake -G "MinGW Makefiles" ..
cmake --build .