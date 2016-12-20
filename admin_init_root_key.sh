#!/bin/bash

PRINTHELP=0

if [ "$#" -lt 4 ]; then
        echo $'\ninstall.sh requires 4 parameters.\n'
        PRINTHELP=1
fi

if [ $PRINTHELP = 1 ] ; then
    echo "admin_init_root_key.sh <NODE1> <NODE2> <NODE3> <NODE4>"
    echo "<NODE1> Name of master, enumerating node"
    echo "<NODE2> Name of slave node connected to Switch Port 2"
    echo "<NODE3> Name of slave node connected to Switch Port 3"
    echo "<NODE4> Name of slave node connected to Switch Port 4"
    echo " "
    echo "NOTE: This script must be executed on NODE1!"
    echo " "
    echo "NOTE: Before executing this script for a user, execute it as root!"
    echo "      This requires entering the root password several times."
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

_user="$(id -u -n)"

cd ~

echo "Generate SSH key for $_user"
echo "Working in $PWD directory"

if [ ! -d ./.ssh -o ! -f ./.ssh/id_rsa.pub ]
then
	echo "Missing local .ssh key. Creating one now"
	mkdir -p .ssh
	ssh-keygen -t rsa -N "" -f .ssh/id_rsa -q
fi

for node in "${ALLNODES[@]}"
do
	echo "==== ${node} ===="
	ssh root@"${node}" "mkdir -p .ssh"
	cat .ssh/id_rsa.pub | ssh root@"${node}" 'cat >> .ssh/authorized_keys'
	ssh root@"${node}" "chmod 700 .ssh; chmod 640 .ssh/authorized_keys"
done

exit 0
