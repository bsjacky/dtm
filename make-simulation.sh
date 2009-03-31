#!/bin/sh

# simple script to configure, compile and install the modules

PWD1=$PWD;
mkdir build;
cd build;
cmake -DCMAKE_VERBOSE_MAKEFILE=1 -DBUILD_MODULES=1 -DCMAKE_INSTALL_PREFIX:PATH=$PWD1 $PWD1;
make install;
cd $PWD1;