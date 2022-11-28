#!/bin/sh

dir_path="/home/tsuyukusa/workspace/compiler02/parsetest/"

for num in $(seq 1 5);
do
  echo "slashitem white00${num}.mpl"
  echo "slashbegin{lstlisting}"
  ./token-list "${dir_path}white00${num}.mpl"
  echo "slashend{lstlisting}"
  echo "\n"
done

# for num in $(seq 10 50);
# do
#   echo $dir
#   ./token-list ${dir_path}white0${num}.mpl
# done