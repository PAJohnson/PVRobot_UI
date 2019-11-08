#!/bin/bash

kill $(ps aux | grep '[s]erver.py')

./server.py DEBUG & sleep 2 && xdg-open 'http://0.0.0.0:5000/index.html'
