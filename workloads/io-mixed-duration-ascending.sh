#!/bin/sh

./pq add ./worksim.py 0.1 1
./pq add ./worksim.py 0.1 2
./pq add ./worksim.py 0.1 3
./pq add ./worksim.py 0.5 4
./pq add ./worksim.py 0.5 5
./pq add ./worksim.py 0.5 6
./pq add ./worksim.py 0.9 7
./pq add ./worksim.py 0.9 8
./pq add ./worksim.py 0.9 9
./pq add ./worksim.py 0.9 10

watch ./pq status
