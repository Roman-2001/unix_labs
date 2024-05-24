#!/bin/bash

echo "Run 100 clients twice" >> result.txt

./test1.sh
./test1.sh

echo "Check descriptor num and sbrk()" >> result.txt
echo "First accept:" >> result.txt
cat /tmp/server.log | grep "Accept" | head -n 1 >> result.txt

echo "Last two accepts:" >> result.txt
cat /tmp/server.log | grep "Accept" | tail -n 2 >> result.txt
