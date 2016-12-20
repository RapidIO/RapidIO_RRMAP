#!/bin/bash

# This script starts RDMA daemon on all computing nodes listed in NODES.
# NOTE:
# RapidIO RDMA software installation path must be identical on all nodes.

#RDMA_ROOT_PATH=/opt/rapidio/cern/rapidio_sw
RDMA_ROOT_PATH=/home/srio/git/rapidio_sw
RIO_CLASS_MPORT_DIR=/sys/class/rio_mport/rio_mport0
# 10.10.10.51 is GRYPHON (MASTER)
# 10.10.10.50 is GIGABYTE (SLAVE)
NODES="10.10.10.51 10.10.10.50"
NUM_ITERATIONS=200

j=NUM_ITERATIONS
for (( i=0; i<NUM_ITERATIONS; i++ ))
do
	echo -n "Iteration " $i
	echo ""

	# Load all drivers FIRST
	for node in $NODES
	do
		# Load the drivers
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

	# Wait for enumeration a few times before proceeding, if necessary
	# (Alex's suggestion)
	for node in $NODES
	do
		ENUM_FAIL_RETRY=1
		while [ $ENUM_FAIL_RETRY -le 3 ]
		do
			RIODEVS=$(ssh root@"$node" "ls /sys/bus/rapidio/devices/")
			if [ -z "$RIODEVS" ]
			then
				echo "   not enumerated. Waiting and checking again"
				sleep 1
				(( ENUM_FAIL_RETRY++ ))
			else
				echo "   RIO devices: "$RIODEVS""
				ENUM_FAIL_RETRY=4
			fi
		done
		if [ $ENUM_FAIL_RETRY -eq 3 ]
		then
			echo "Enumeration failure after retries!"
		fi
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

	# Now check that everything is still running OK
	
	OK=1	# Set OK to true before the checks

	#For each node check that all is well
	for node in $NODES
	do
		# Display note name
		echo "+++ $node +++"

		# Check that the node was properly enumerated
		RIODEVS=$(ssh root@"$node" "ls /sys/bus/rapidio/devices/")
		if [ -z "$RIODEVS" ]
		then
			echo "   not enumerated"
			OK=0
		else
			echo "   RIO devices: "$RIODEVS""
		fi

		# Display the 'destid' for the node
		DESTID=$(ssh root@"$node" "cat $RIO_CLASS_MPORT_DIR/device/port_destid 2>/dev/null")
		echo "   mport destID=$DESTID"

		# Check that rio_mport_cdev was loaded
		RIO_MPORT_CDEV=$(ssh root@"$node" "lsmod | grep rio_mport_cdev")
		if [ -z "$RIO_MPORT_CDEV" ]
		then
			echo "   rio_mport_cdev *NOT* loaded"
			OK=0
		else
			echo "   rio_mport_cdev loaded"
		fi

		# Check that rio_cm was loaded
		RIO_CM=$(ssh root@"$node" "lsmod | grep rio_cm")
		if [ -z "$RIO_CM" ]
		then
			echo "   rio_cm     *NOT* loaded"
			OK=0
		else
			echo "   rio_cm     loaded"
		fi

		# Check that fmd is running
		FMD_PID=$(ssh root@"$node" pgrep fmd)
		if [ -z "$FMD_PID" ]
		then
			echo "   FMD   *NOT* running"
			OK=0
		else
			echo "   FMD   is running PID=$FMD_PID"
		fi

		# Check that rdmad is running
		RDMAD_PID=$(ssh root@"$node" pgrep rdmad)
		if [ -z "$RDMAD_PID" ]
		then
			echo "   RDMAD *NOT* running"
			OK=0
		else
			echo "   RDMAD is running PID=$RDMAD_PID"
		fi

		# Check that rsktd is running
		RSKTD_PID=$(ssh root@"$node" pgrep rsktd)
		if [ -z "$RSKTD_PID" ]
		then
			echo "   RSKTD *NOT* running"
			OK=0
		else
			echo "   RSKTD is running PID=$RSKTD_PID"
		fi
	done

	# If there is any failure then stop so we can examine the logs
	if [ $OK -eq 0 ]
	then
		echo "	Something failed. Ending test. Check logs on failed node!"
		echo ""
		j=$i
		i=NUM_ITERATIONS
	else
		echo "	Everything worked. Retrying, but cleaning up first"
		echo ""

		NODES="10.10.10.50 10.10.10.51"

		# For each node, kill RSKTD RDMAD and FMD
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
			echo "Killing fmd on $node RDMAD PID=$THE_PID"
			for proc in $THE_PID
			do
				ssh root@"$node" "kill -s 2 $proc"
			done
		done

		# Unload all drivers from all nodes
		for node in $NODES
		do
			# Unload drivers
			echo "Unloading drivers on $node "
			ssh root@"$node" "modprobe -r rio_mport_cdev"
			sleep 1
			ssh root@"$node" "modprobe -r rio_cm"
			sleep 1
		done
	fi # 	if [ $OK -eq 0 ]
done #for (( i=0; i<NUM_ITERATIONS; i++ ))

echo -n "Iterations done: " $j
echo ""
echo "Goodbye"
echo ""

exit 0

