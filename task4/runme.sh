#!/bin/bash

make server
make client
chmod +x test1.sh test2.sh test3.sh
./server &
sleep 1
touch result.txt
echo "First task:" > result.txt
./test1.sh
echo "" >> result.txt
echo "Second task:" >> result.txt
./test2.sh
echo "" >> result.txt
echo "Third task:" >> result.txt
./test3.sh
rm /tmp/server* /tmp/sock*
