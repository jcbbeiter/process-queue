#!/bin/sh

./pq add ./worksim.py 0.1 5
./pq add ./worksim.py 0.1 5
./pq add ./worksim.py 0.1 5
./pq add ./worksim.py 0.5 5
./pq add ./worksim.py 0.5 5
./pq add ./worksim.py 0.5 5
./pq add ./worksim.py 0.9 5
./pq add ./worksim.py 0.9 5
./pq add ./worksim.py 0.9 5
./pq add ./worksim.py 0.9 5

watch ./pq status
