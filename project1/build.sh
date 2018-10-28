#!/bin/bash

UNI_OPTS="-std=c++11 -O3 -march=native -I./../tomasulo"

echo "Compiling for ${OSTYPE}"

case "$OSTYPE" in
    linux-gnu)
        g++ -o main main.cpp ${UNI_OPTS} -I./../misc
    ;;

    msys)
        g++ -o main main.cpp ${UNI_OPTS}
    ;;
esac
