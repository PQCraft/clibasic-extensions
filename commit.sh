#!/bin/bash
./build-all.sh || exit 1
./clean.sh
git add */ *.sh README.md
git commit -S -m 'Update extensions'
git push
