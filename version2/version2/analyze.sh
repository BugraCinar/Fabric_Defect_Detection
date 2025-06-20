#!/bin/bash

mkdir -p analyze

# O1
echo "[O1 - TIME REPORT]" > analyze/version2_time_O1.txt
g++ -std=c++17 -O1 -o version2_O1 version2.cpp
/usr/bin/time -v -o analyze/version2_time_O1.txt -a ./version2_O1

# O2
echo "[O2 - TIME REPORT]" > analyze/version2_time_O2.txt
g++ -std=c++17 -O2 -o version2_O2 version2.cpp
/usr/bin/time -v -o analyze/version2_time_O2.txt -a ./version2_O2

# O3
echo "[O3 - TIME REPORT]" > analyze/version2_time_O3.txt
g++ -std=c++17 -O3 -o version2_O3 version2.cpp
/usr/bin/time -v -o analyze/version2_time_O3.txt -a ./version2_O3

echo "[O3 - VALGRIND REPORT]" > analyze/version2_valgrind_O3.txt
valgrind --leak-check=full --log-file=analyze/version2_valgrind_O3.txt ./version2_O3