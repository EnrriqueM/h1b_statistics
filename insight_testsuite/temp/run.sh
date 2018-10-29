#!/bin/bash
#
# Use this shell script to compile (if necessary) your code and then execute it. Below is an example of what might be found in this file if your program was written in Python

file="./input/H1B_FY_2014.csv"
if [ $# == 0 ]; then
	echo 'No file entered. Using default input/H1B_FY_2014.csv'
else
	file="$1"
fi

echo Compiling main.cpp...
g++ src/main.cpp -o src/main
echo Done
echo Executing main
src/main $file
exit 0
