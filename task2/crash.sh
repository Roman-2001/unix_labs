#!/bin/bash

make

touch test
./locker test &
sleep 0.1
rm -f test.lck

wait
echo "removing test"

touch test1
./locker test1 &
sleep 0.1
echo "123" > test1.lck

wait
echo "occupied .lck-file test"
rm -f test1.lck
rm -f test
rm -f test1
