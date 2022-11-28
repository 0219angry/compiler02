#!/bin/sh

for i in $(seq 1 9);
do
echo "white0${i}"
cp white000.mpl white00$i.mpl
done

for i in $(seq 10 50);
do
echo "white0${i}"
cp white000.mpl white0$i.mpl
done