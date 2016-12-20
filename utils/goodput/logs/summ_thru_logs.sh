#!/bin/bash

## This script grabs the goodput figures from a file, along with CPU occupancy
## It also displays the size of the transfer

SUFFIX=log
OSUFFIX=res

if [ -n "$1" ]; then
	SUFFIX=$1
fi

if [ -n "$2" ]; then
	OSUFFIX=$2
fi

function summarize_thruput_log {
	IN_FILE=$1

	TEMP_FILE1=$1'.temp1'
	TEMP_FILE2=$1'.temp2'
	TEMP_FILE3=$1'.temp3'
	OUT_FILE=$1'.'$OSUFFIX

	echo $'\nProcessing througput log file : ' $IN_FILE 
	echo $'Output filename is            : ' $OUT_FILE 
	
	## Get CPU OCC measurements, performance measurements, and labels
	awk '/Kernel/{li=$0;getline;print li $0}
	/Total/{print}
	/echo/{print}' $IN_FILE  > $TEMP_FILE1
	
	### Contatenate groups of 3 lines together
	
	awk '
	BEGIN {getline; l1 = $0; getline; l2 = $0}
	{print l1 " " l2 " " $0; l1 = l2; l2 = $0}
	' $TEMP_FILE1 > $TEMP_FILE2
	
	## Only keep lines with echo, Total, and Kernel
	grep -E '(^echo.*Total.*Kernel)' $TEMP_FILE2 > $TEMP_FILE3
	
	## Reformat each line, and add a header...
	awk '
	BEGIN {print "SIZE        MBps     Gbps   LinkOcc  AvailCPU  UserCPU  KernCPU CPU OCC %"}
	{printf "%8s %9s %8s %8s %8s %8s %8s %8s\n", $5, $9, $10, $12, $17, $18, $19, $20}
	' $TEMP_FILE3 > $OUT_FILE

	rm $TEMP_FILE1 $TEMP_FILE2 $TEMP_FILE3
	cat $OUT_FILE
}

dir_list="$(ls *thru_*.${SUFFIX} | grep -v done)"

if [ ${#dir_list[@]} -eq 0 ]; then
        exit
fi

for f in ${dir_list[@]}; do
        summarize_thruput_log ${f}
done
