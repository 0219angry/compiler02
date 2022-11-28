#!/bin/sh

dir_path="/home/tsuyukusa/workspace/compiler02/parsetest/"

for num in $(seq 0 9);
do
  echo "========================================================================================="
  echo "white00${num}.mpl"
  ./token-list "${dir_path}white00${num}.mpl"
done

for num in $(seq 10 34);
do
  echo "========================================================================================="
  echo "white00${num}.mpl"
  ./token-list ${dir_path}white0${num}.mpl
done