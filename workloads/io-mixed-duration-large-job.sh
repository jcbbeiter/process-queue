#!/bin/sh

./pq add ./worksim.py 0.1 20
for i in $(seq 9); do
    ./pq add ./worksim.py 0.$i 5
done

./pq add ./worksim.py 0.1 10

for i in $(seq 9); do
    ./pq add ./worksim.py 0.$i 5
done

./pq add ./worksim.py 0.1 10

for i in $(seq 9); do
    ./pq add ./worksim.py 0.$i 2
done

./pq add ./worksim.py 0.1 5

watch ./pq status
