#!/bin/bash

mkdir -p analyze

echo "[O1 - TIME REPORT]" > analyze/version3_time_O1.txt
g++ -std=c++17 -O1 -o version3_O1 version3.cpp group_worker.cpp thread_worker.cpp -pthread
/usr/bin/time -v -o analyze/version3_time_O1.txt -a ./version3_O1

echo "[O2 - TIME REPORT]" > analyze/version3_time_O2.txt
g++ -std=c++17 -O2 -o version3_O2 version3.cpp group_worker.cpp thread_worker.cpp -pthread
/usr/bin/time -v -o analyze/version3_time_O2.txt -a ./version3_O2

echo "[O3 - TIME REPORT]" > analyze/version3_time_O3.txt
g++ -std=c++17 -O3 -pg -o version3_O3 version3.cpp group_worker.cpp thread_worker.cpp -pthread
/usr/bin/time -v -o analyze/version3_time_O3.txt -a ./version3_O3

echo "[O3 - VALGRIND REPORT]" > analyze/version3_valgrind_O3.txt
valgrind --leak-check=full --log-file=analyze/version3_valgrind_O3.txt ./version3_O3

echo "[O3 - GPROF REPORT]" > analyze/version3_gprof_O3.txt
gprof ./version3_O3 gmon.out > analyze/version3_gprof_O3.txt

