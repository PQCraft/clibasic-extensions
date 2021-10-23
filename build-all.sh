#!/bin/bash
for dir in ./*/; do
    ./build.sh "$dir" || exit 1
done
