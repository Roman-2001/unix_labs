#!/bin/bash
rm -f fileA fileA.gz file B fileB.gz fileC fileD
python3 file_creater.py;
gcc myprogram.c -o myprogram;
./myprogram fileA fileB;
gzip -k fileA fileB;
gzip -cd fileB.gz | ./myprogram fileC;
./myprogram fileA fileD 100;
stat -c "%n %s" fileA fileA.gz fileB fileB.gz fileC fileD;
