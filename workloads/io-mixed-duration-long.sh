#!/bin/sh

./pq add ./worksim.py 0.1 30
./pq add ./worksim.py 0.1 30
./pq add ./worksim.py 0.1 30
./pq add ./worksim.py 0.5 30
./pq add ./worksim.py 0.5 30
./pq add ./worksim.py 0.5 30
./pq add ./worksim.py 0.9 30
./pq add ./worksim.py 0.9 30
./pq add ./worksim.py 0.9 30
./pq add ./worksim.py 0.9 30

watch ./pq status
