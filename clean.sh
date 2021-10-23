#!/bin/bash
for dir in ./*/; do
    rm -f "$dir/"*.o
    rm -f "$dir/"*.so
done
