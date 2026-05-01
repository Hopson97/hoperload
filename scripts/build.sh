#!/bin/bash

target_release() {
    cd release
    cmake -DCMAKE_BUILD_TYPE=Release ../..
    make
    echo "Built target in build/release/"
    cd ../..
}

target_debug() {
    cd debug 
    cmake -DCMAKE_BUILD_TYPE=Debug ../..
    make
    echo "Built target in build/debug/"
    cd ../..
}

# Create folder for distribution
if [ "$1" = "release" ]
then
    if [ -d "$hoperload" ]
    then
        rm -rf -d hoperload
    fi

    mkdir -p hoperload
fi

# Creates the folder for the buildaries
mkdir -p hoperload 
mkdir -p hoperload/assets
mkdir -p build
mkdir -p build/release
mkdir -p build/debug
cd build

# Builds target
if [ "$1" = "release" ]
then
    target_release
    cp build/release/hoperload hoperload/hoperload
else
    target_debug
fi

cp -R assets hoperload/
