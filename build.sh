#!/bin/bash

echob() {
    printf "\e[1m"
    echo "$@"
    printf "\e[0m"
}

OLDDIR="$(pwd)"
[ -z "$1" ] && exit 1
echob "Building $1..."
cd "$1"
rm -f *.so
for FILE in *.c; do
    gcc -I"$OLDDIR" $(cat options.txt 2> /dev/null) -fpic "$FILE" -c
done
gcc $(cat options.txt 2> /dev/null) -shared -Wl,--export-dynamic *.o -o "cbext${PWD##*/}.so"
rm -f *.o
cd "$OLDDIR"
