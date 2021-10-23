#!/bin/bash
./build-all.sh
./clean.sh
git add *
git commit -S -m 'Update extensions'
git push
