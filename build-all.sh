#!/bin/bash
for dir in ./*/; do
    ./build.sh "$dir"
done
