#!/bin/bash
echob() {
    printf "\e[1m"
    echo "$@"
    printf "\e[0m"
}
OLDDIR="$(realpath "$(dirname "$0")")"
[ -z "$1" ] && exit 1
if [ "$(realpath "$1")" = "$(pwd)" ]; then
    echob "Building..."
else
    echob "Building $1..."
fi
cd "$1"
rm -f *.so
for FILE in *.c; do
    gcc -I"$OLDDIR" $(cat options.txt 2> /dev/null) -fpic "$FILE" -c || exit 1
done
gcc -L"$OLDDIR" $(cat options.txt 2> /dev/null) -shared -Wl,--export-dynamic *.o -o "cbext${PWD##*/}.so" || exit 1
rm -f *.o
cd "$OLDDIR"
