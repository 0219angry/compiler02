#!/bin/sh

dir_path="/home/tsuyukusa/workspace/compiler02/parsetest/"

for num in $(seq 1 5);
do
  echo "========================================================================================="
  echo "${dir_path}white00${num}.mpl"
  ./token-list "${dir_path}white00${num}.mpl"
done

# for num in $(seq 10 50);
# do
#   echo $dir
#   ./token-list ${dir_path}white0${num}.mpl
# done