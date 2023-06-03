#!/bin/bash

file=$1
flag=$2

if [ $flag = "r" ]; then
    cat $file
elif [ $flag = "w" ]; then
    echo "175 MYFILE" > $file
fi
