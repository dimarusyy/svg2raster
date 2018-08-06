#!/bin/sh

GM_CONFIG=$1
case $2 in
    --ldflags)
    echo -n `${GM_CONFIG} --ldflags`
    ;;
    --libs)
    echo -n `${GM_CONFIG} --libs | sed "s/ -l/;/g" | cut -d ';' -f3-`
    ;;
esac