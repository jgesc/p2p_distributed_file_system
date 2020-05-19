#!/usr/bin/env bash

mkdir test_scene
cd test_scene

../bin/dhtnode 8000 &
for i in {1..256}
do
	port=$((8000 + i))
	top=$(($port - 1))
	seed=`shuf -i 8000-$top -n 1`
	../bin/dhtnode $port localhost $seed &
	sleep 0.1
done