#!/bin/bash

#  This script adds a new user on all RIO cluster nodes.

#  Warning:
#  ======== 
#  Please note that the master node (one where this script is executed)
#+ must be first entry in the list of nodes.

PRINTHELP=0

if [ $(id -u) -ne 0 ]; then
	echo "Only root may add a user"
	exit 1
fi

if [ "$#" -lt 7 ]; then
        echo $'\nadmin_user_add.sh requires 7 parameters.\n'
        PRINTHELP=1
fi

if [ $PRINTHELP = 1 ] ; then
    echo "admin_user_add.sh <NODE1> <NODE2> <NODE3> <NODE4> <user> <pass> <grp>"
    echo "<NODE1> Name of master, enumerating node"
    echo "<NODE2> Name of slave node connected to Switch Port 2"
    echo "<NODE3> Name of slave node connected to Switch Port 3"
    echo "<NODE4> Name of slave node connected to Switch Port 4"
    echo "<user>  Name of the user to add"
    echo "<pass>  Password for the user"
    echo "<grp>   Unix file ownership group for this user."
    echo "        This group should have access to the RapidIO software."
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
PASS=$6
GRP=$7

echo "Adding user $NEW_USER pwd:$PASS on nodes:"

for node in "${ALLNODES[@]}"
do
	echo "${node}"

	ssh root@"${node}" "useradd -m -G ${GRP} ${NEW_USER}"
	ssh root@"${node}" "usermod -a -G wheel ${NEW_USER}"
	ssh root@"${node}" "echo $NEW_USER:$PASS | chpasswd"

done

sudo -u $NEW_USER sh -c "cd /home/$NEW_USER; pwd; mkdir -p .ssh; ssh-keygen -t rsa -N \"\" -f .ssh/id_rsa"
cd /home/$NEW_USER
chmod 700 .ssh
pwd

echo "Register as root on nodes:"

for node in "${ALLNODES[@]}"
do
	echo "${node}"
	cat .ssh/id_rsa.pub | ssh root@${node} 'cat >> .ssh/authorized_keys'
done

