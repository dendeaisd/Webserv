#!/bin/bash

for i in {1..10}; do
  curl -d "test$i" http://localhost:8080 &
done

wait
echo "All requests are done"
