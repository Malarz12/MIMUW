#!/bin/bash

if [ $# -ne 2 ]
then
  echo "Usage: $0 <port> <number of servers>"
  exit 1
fi

echo "launching $2 servers:"

for i in $(seq $2)
do
  ./reuse-server $1 &
done
