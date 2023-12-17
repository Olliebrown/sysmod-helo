#!/bin/bash

# Run make in all folders that have a Makefile
for folder in *; do
    if [ -e "$folder/Makefile" ]; then
        pushd "$folder"
        make $1
        popd
    fi
done
