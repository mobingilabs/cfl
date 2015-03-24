#!/bin/bash

pushd coco-r-cpp
make
popd
coco-r-cpp/Coco cfl.atg
g++ -g -rdynamic -Wall -std=c++11 -stdlib=libc++ `ls *.cpp` -o cfl
