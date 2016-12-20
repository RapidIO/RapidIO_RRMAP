#!/bin/bash

PRINTHELP=0

if [ "$#" -lt 5 ]; then
        echo $'\nadmin_init_dir.sh requires 5 parameters.\n'
        PRINTHELP=1
fi

if [ $PRINTHELP = 1 ] ; then
    echo "admin_init_dir.sh <NODE1> <NODE2> <NODE3> <NODE4> <group>"
    echo "<NODE1> Name of master, enumerating node"
    echo "<NODE2> Name of slave node connected to Switch Port 2"
    echo "<NODE3> Name of slave node connected to Switch Port 3"
    echo "<NODE4> Name of slave node connected to Switch Port 4"
    echo "If any of <NODE2> <NODE3> <NODE4> is \"none\", the node is ignored."
    echo "<group> Unix file ownership group which should have access to"
    echo "        the RapidIO software"
    exit
fi

ALLNODES=( $1 )

if [ $2 != 'none' ]; then
        ALLNODES[1]=$2
fi


if [ $3 != 'none' ]; then
        ALLNODES[2]=$3
fi

if [ $4 != 'none' ]; then
        ALLNODES[3]=$4
fi

GRP=$5

for node in "${ALLNODES[@]}"
do
	echo $i
	ssh root@"${node}" "groupadd $GRP"
	ssh root@"${node}" "mkdir -p /opt/rapidio"
	ssh root@"${node}" "chmod g+w /opt/rapidio"
	ssh root@"${node}" "chgrp $GRP /opt/rapidio"

done

exit 0
