#!/bin/bash

# MPI variables
HOSTFILE=nodes
NPROCS=4


# call the rtgen program and get a list of supported hashes return to the array
HashAlgorithms=( $(./rtgen | grep "hash_algorithm:" | sed 's/.*: //')  )

#Loop through the array and run a benchmark on each hash outputing to CSV
for i in "${HashAlgorithms[@]}"
do
   :
   # run multi-platform
   mpirun -np $NPROCS -bycore -hostfile $HOSTFILE $HOME/bin/rtgen $i all 1 15 0 -bench-csv
done