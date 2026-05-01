#!/bin/bash

if [ "$1" = "release" ]
then
    ./build/release/hoperload
else
    ./build/debug/hoperload
fi