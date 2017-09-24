#!/bin/sh

./pq add ./worksim.py 0.1 30
sleep 15
for i in $(seq 9); do
    ./pq add ./worksim.py 0.$i 5
done
sleep 5
for i in $(seq 9); do
    ./pq add ./worksim.py 0.$i 1
done

watch ./pq status
