#!/bin/bash
RDMA_ROOT_PATH=/home/sherif/git/rapidio_sw
RIO_CLASS_MPORT_DIR=/sys/class/rio_mport/rio_mport0

# Temporary script to speed up RSKT test apps development.
NODES="GRY01 GRY02 GRY03 GRY04"

# Load drivers on each node
for node in $NODES
do
	ssh root@"$node" "modprobe rio_cm"
	sleep 1
	ssh root@"$node" "modprobe rio_mport_cdev"
	sleep 1
done

# Start Fabric Management Daemon on each node
for node in $NODES
do
	DESTID=$(ssh root@"$node" "cat $RIO_CLASS_MPORT_DIR/device/port_destid")
	echo "Starting fmd on $node destID=$DESTID"
	ssh root@"$node" "screen -dmS fmd $RDMA_ROOT_PATH/fabric_management/daemon/fmd -l7"
	sleep 1
	FMD_PID=$(ssh root@"$node" pgrep fmd)
	echo "$node fmd pid=$FMD_PID"
done

# Start RDMAD on each node
for node in $NODES
do
	DESTID=$(ssh root@"$node" "cat $RIO_CLASS_MPORT_DIR/device/port_destid")
	echo "Start rdmad on $node destID=$DESTID"
	ssh root@"$node" "screen -dmS rdmad $RDMA_ROOT_PATH/rdma/rdmad"
	sleep 1
	RDMAD_PID=$(ssh root@"$node" pgrep rdmad)
	echo "$node rdmad pid=$RDMAD_PID"
done

# Start RSKTD on each node
for node in $NODES
do
	DESTID=$(ssh root@"$node" "cat $RIO_CLASS_MPORT_DIR/device/port_destid")
	echo "Start rsktd on $node destID=$DESTID"
	ssh root@"$node" "screen -dmS rsktd $RDMA_ROOT_PATH/rdma/rskt/daemon/rsktd -l7"
	sleep 1
	RSKTD_PID=$(ssh root@"$node" pgrep rsktd)
	echo "$node rsktd pid=$RSKTD_PID"
done

