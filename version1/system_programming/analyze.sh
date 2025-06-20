#!/bin/bash

# analyze klasörü oluşturulmazsa, oluştur
mkdir -p analyze

# TIME ölçümü: user/sys/real zamanları kaydet
echo "[TIME REPORT]" > analyze/version1_time_report.txt
/usr/bin/time -v -o analyze/version1_time_report.txt ./version1

# VALGRIND ölçümü: bellek hatası ve sızıntı kontrolü
valgrind --leak-check=full --log-file=analyze/version1_valgrind.txt ./version1
