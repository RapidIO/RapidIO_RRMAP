#!/bin/bash

# Temporary script to speed up RSKT test apps development.

RDMA_ROOT_PATH=/home/sherif/git/rapidio_sw
RIO_CLASS_MPORT_DIR=/sys/class/rio_mport/rio_mport0

NODES="GRY12 GRY11 GRY10 GRY09"

for node in $NODES
do
	# Kill RSKTD
	THE_PID=$(ssh root@"$node" pgrep rsktd)
	echo "Killing rsktd on $node RSKTD PID=$THE_PID"
	for proc in $THE_PID
	do
		ssh root@"$node" "kill -s 2 $proc"
	done

	# Kill RDMAD
	THE_PID=$(ssh root@"$node" pgrep rdmad)
	echo "Killing rdmad on $node RDMAD PID=$THE_PID"
	for proc in $THE_PID
	do
		ssh root@"$node" "kill -s 2 $proc"
	done

	# Kill FMD
	THE_PID=$(ssh root@"$node" pgrep fmd)
	echo "Killing fmd on $node FMD PID=$THE_PID"
	for proc in $THE_PID
	do
		ssh root@"$node" "kill -s 2 $proc"
	done

	sleep 3

	# Unload drivers
	ssh root@"$node" "modprobe -r rio_cm"
	sleep 1
	ssh root@"$node" "modprobe -r rio_mport_cdev"
	sleep 1
done

