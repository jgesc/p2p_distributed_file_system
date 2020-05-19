#!/usr/bin/env bash

mkdir test_scene
cd test_scene
../bin/dhtserver 8000 &
sleep 0.5
../bin/dhtserver 8001 localhost 8000 &
sleep 0.5
../bin/dhtserver 8002 localhost 8000 &
sleep 0.5
../bin/dhtserver 8003 localhost 8001 &
sleep 0.5
../bin/dhtserver 8004 localhost 8002 &
sleep 0.5
../bin/dhtserver 8005 localhost 8002 &
sleep 0.5
../bin/dhtserver 8006 localhost 8003 &
sleep 0.5
../bin/dhtserver 8007 localhost 8006 &
