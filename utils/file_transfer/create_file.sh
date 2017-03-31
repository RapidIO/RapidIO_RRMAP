#!/bin/bash

if [ -z $1 ] || [ -z $2 ]
then
	echo "create_file <file_name> <size_in_bytes>"
	echo " "
	echo "Example: create_file.sh big_rnd.txt 128M"
	exit
fi

if [ -f $1 ]
then
	echo "file already exists, aborting."
	exit
fi

dd if=/dev/urandom of=$1 bs=$2 count=1

ls -la $1
