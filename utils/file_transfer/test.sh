#!/bin/bash

PRINTHELP=0
re='^[0-9]+$'

if [ "$#" -lt 4 ]; then
	echo $'\ntest.sh requires 4 parameters.\n'
	PRINTHELP=1
elif [ ! -f $1 ]; then
	echo $'\nSource file '$1$' does not exist.\n'
	PRINTHELP=1
elif [ ! -s $1 ]; then
	echo $'\nSource file '$1$' is empty.\n'
	PRINTHELP=1
elif ! [[ $3 =~ $re ]] ; then
	echo $'\nDestid must be a decimal number.\n'
	PRINTHELP=1
elif ! [[ $4 =~ $re ]] ; then
	echo $'\nCount must be a decimal number\n'
	PRINTHELP=1
fi

if [ $PRINTHELP = 1 ] ; then
    echo "test <srcfile> <dstfile> <destid> <count>"
    echo "<srcfile> Name of file to transfer to fxfr_server"
    echo "<dstfile> Name of file at the target, has iteration number appended"
    echo "<destid>  Decimal destination ID number, printed by st command on"
    echo "          file transfer server console."
    echo "<count>   Number of times the file should be transferred."
    exit
fi

SRC_FILE=$1
DST_FILE=$2
DESTID=$3
MAX_ITER=$4

for ((i = 1; i<= $MAX_ITER; i++ ))
do
	DST_FILE_NAME=${DST_FILE}${i}
	echo ${SRC_FILE} ${DST_FILE_NAME} ${DESTID}
	./rftp ${SRC_FILE} ${DST_FILE_NAME} ${DESTID}
	if [ $? -ne 0 ]; then
		echo "Iteration " $i " failed, exiting..."
		exit
	fi
done

