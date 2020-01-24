#!/bin/bash

echo "Running disc"
# Create disc
./filesystem mkfs -s 1000000 -b 4096 # 945MB disc, 4kB each block

# Add 10 files
size=10000
filename="filename"
for i in {1..10}
do
  size=$((size+10000))
  file="${filename}${i}"
  ./filesystem touch $file $size
done
./filesystem ls

# remove 10 files
for i in {1..10}
do
  file="${filename}${i}"
  ./filesystem rm $file
done
./filesystem ls

# Show that the disc is empty

./filesystem info

size=10000
filename="filename"
for i in {1..10}
do
  size=$((size+10000))
  file="${filename}${i}"
  ./filesystem touch $file $size
done
./filesystem ls

# External file copy
filename="file"
./filesystem extcp -s $filename -d $filename

./filesystem ls

./filesystem get $filename