#!/bin/sh

./pq add ./worksim.py 0.1 10
./pq add ./worksim.py 0.1 9
./pq add ./worksim.py 0.1 8
./pq add ./worksim.py 0.5 7
./pq add ./worksim.py 0.5 6
./pq add ./worksim.py 0.5 5
./pq add ./worksim.py 0.9 4
./pq add ./worksim.py 0.9 3
./pq add ./worksim.py 0.9 2
./pq add ./worksim.py 0.9 1

watch ./pq status
