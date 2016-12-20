#!/bin/bash

#  This script adds a new user on all RIO cluster nodes.

#  Warning:
#  ======== 
#  Please note that the master node (one where this script is executed)
#+ must be first entry in the list of nodes.

PRINTHELP=0

if [ $(id -u) -ne 0 ]; then
	echo "Only root may delete a user"
	exit 1
fi

if [ "$#" -lt 5 ]; then
        echo $'\nadmin_user_del requires 5 parameters.\n'
        PRINTHELP=1
fi

if [ $PRINTHELP = 1 ] ; then
    echo "admin_user_del <NODE1> <NODE2> <NODE3> <NODE4> <user>"
    echo "<NODE1> Name of master, enumerating node"
    echo "<NODE2> Name of slave node connected to Switch Port 2"
    echo "<NODE3> Name of slave node connected to Switch Port 3"
    echo "<NODE4> Name of slave node connected to Switch Port 4"
    echo "<user>  Name of the user to add"
    echo " "
    echo "If any of <NODE2> <NODE3> <NODE4> is \"none\", the node is ignored."
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

NEW_USER=$5

echo "Deleting user $NEW_USER"

for node in "${ALLNODES[@]}"
do
	echo "${node}"

	ssh root@"${node}" "killall -KILL -u ${NEW_USER}; userdel -r ${NEW_USER}"

done

