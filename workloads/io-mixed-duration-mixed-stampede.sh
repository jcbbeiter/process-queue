#!/bin/sh

./pq add ./worksim.py 0.1 1
sleep 1
./pq add ./worksim.py 0.1 5
./pq add ./worksim.py 0.1 30
sleep 10
./pq add ./worksim.py 0.5 1
sleep 1
./pq add ./worksim.py 0.5 5
./pq add ./worksim.py 0.5 30
sleep 10
./pq add ./worksim.py 0.9 1
sleep 1
./pq add ./worksim.py 0.9 5
./pq add ./worksim.py 0.9 30
sleep 10
./pq add ./worksim.py 0.9 1

watch ./pq status
