#!/bin/bash

if [ "$1" = "release" ]
then
    ./build/release/bin/hoperload 
else
    ./build/debug/bin/hoperload 
fi