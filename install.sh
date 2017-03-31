#!/bin/bash

# Files required for installation
# Note the names of these file name (different root) are also used by make_install.sh
#
REMOTE_ROOT="/opt/rapidio/.install"
LOCAL_SOURCE_ROOT="$(pwd)"
SCRIPTS_PATH=$LOCAL_SOURCE_ROOT/install

NODEDATA_FILE="nodeData.txt"
SRC_TAR="rapidio_sw.tar"
TMPL_FILE="config.tmpl"

MY_USERID=root

PGM_NAME=install.sh
PGM_NUM_PARMS=8

# Validate input
#
PRINTHELP=0

if [ "$#" -lt $PGM_NUM_PARMS ]; then
    echo $'\n$PGM_NAME requires $PGM_NUM_PARMS parameters.\n'
    PRINTHELP=1
else
    SERVER=$1
    MASTER=$2
    ALLNODES=( $2 $3 $4 $5 )
    MEMSZ=$6
    SW_TYPE=$7
    GRP=$8
    REL=$9

    if [ $MEMSZ != 'mem34' -a $MEMSZ != 'mem50' -a $MEMSZ != 'mem66' ] ; then
        echo $'\nmemsz parameter must be mem34, mem50, or mem66.\n'
        PRINTHELP=1
    fi

    MASTER_CONFIG_FILE=$SCRIPTS_PATH/$SW_TYPE-master.conf
    MASTER_MAKE_FILE=$SCRIPTS_PATH/$SW_TYPE-master-make.sh

    if [ ! -e "$MASTER_CONFIG_FILE" ] || [ ! -e "$MASTER_MAKE_FILE" ]
    then
        echo $'\nSwitch type \"$SW_TYPE\" configuration support files do not exist.\n'
        PRINTHELP=1
    fi
fi

if [ $PRINTHELP = 1 ] ; then
    echo "$PGM_NAME <SERVER> <NODE1> <NODE2> <NODE3> <NODE4> <memsz> <sw> <group> <rel>"
    echo "<SERVER> Name of the node providing the files required by installation"
    echo "<NODE1>  Name of master, enumerating node"
    echo "<NODE2>  Name of slave node connected to Switch Port 2"
    echo "<NODE3>  Name of slave node connected to Switch Port 3"
    echo "<NODE4>  Name of slave node connected to Switch Port 4"
    echo "If any of <NODE2> <NODE3> <NODE4> is \"none\", the node is ignored."
    echo "<memsz>  RapidIO memory size, one of mem34, mem50, mem66"
    echo "         If any node has more than 8 GB of memory, MUST use mem50"
    echo "<sw>     Type of switch the four nodes are connected to."
    echo "         Files exist for the following switch types:"
    echo "         tor  - Prodrive Technologies Top of Rack Switch"
    echo "         cps  - StarBridge Inc RapidExpress Switch"
    echo "         auto - configuration determined at runtime"
    echo "         rxs  - StarBridge Inc RXS RapidExpress Switch"
    echo "<group>  Unix file ownership group which should have access to"
    echo "         the RapidIO software"
    echo "<rel>    The software release/version to install."
    echo "         If no release is supplied, the current release is installed."
    exit
fi


# Only proceed if all nodes can be reached
#
echo "Prepare for installation..."
echo "Checking connectivity..."
OK=1
ping -c 1 $SERVER > /dev/null
if [ $? -ne 0 ]; then
    echo "    $SERVER not accessible"
    OK=0
else
    echo "    $SERVER accessible."
fi

for host in "${ALLNODES[@]}"
do
    [ "$host" = 'none' ] && continue;
    [ "$host" = "$SERVER" ] && continue;
    ping -c 1 $host > /dev/null
    if [ $? -ne 0 ]; then
        echo "    $host not accessible"
        OK=0
    else
        echo "    $host accessible."
    fi
done

if [ $OK -eq 0 ]; then
    echo "\nCould not connect to all nodes, exiting..."
    exit
fi


echo "Creating install files for $SERVER..."
# First create the files that would be available on the server
#
TMP_DIR="/tmp/$$"
rm -rf $TMP_DIR;mkdir -p $TMP_DIR

# Create nodeData.txt
#
let c=0;
for host in "${ALLNODES[@]}"; do
    let c=c+1;
    [ "$host" = 'none' ] && continue
    LINE="$host $host node$c"
    if [ $c -eq 1 ] ; then
        echo "master $LINE" >> $TMP_DIR/$NODEDATA_FILE
        MASTER=$host
    else
        echo "slave $LINE" >> $TMP_DIR/$NODEDATA_FILE
    fi
done

# Create the source.tar
#
pushd $LOCAL_SOURCE_ROOT &> /dev/null
make clean &>/dev/null
tar -cf $TMP_DIR/$SRC_TAR * .git* &>/dev/null
popd &> /dev/null

# Copy the template file
#
cp $MASTER_CONFIG_FILE $TMP_DIR/$TMPL_FILE

# Transfer the files to the server
#
echo "Transferring install files to $SERVER..."
SERVER_ROOT="/opt/rapidio/.server"
ssh $MY_USERID@"$SERVER" "rm -rf $SERVER_ROOT;mkdir -p $SERVER_ROOT"
scp $TMP_DIR/* $MY_USERID@"$SERVER":$SERVER_ROOT/. > /dev/null
ssh $MY_USERID@"$SERVER" "chown -R root.$GRP $SERVER_ROOT"
rm -rf $TMP_DIR

# Transfer the make_install.sh script to a known location on the target machines
#
for host in "${ALLNODES[@]}"; do
    [ "$host" = 'none' ] && continue;
    echo "Transferring install script to $host..."
    ssh $MY_USERID@"$host" "rm -rf $REMOTE_ROOT;mkdir -p $REMOTE_ROOT/script"
    scp $SCRIPTS_PATH/make_install_common.sh $MY_USERID@"$host":$REMOTE_ROOT/script/make_install_common.sh > /dev/null
    if [ "$host" = "$MASTER" ]; then
        scp $MASTER_MAKE_FILE $MY_USERID@"$host":$REMOTE_ROOT/script/make_install.sh > /dev/null
    else
        scp $SCRIPTS_PATH/make_install-slave.sh $MY_USERID@"$host":$REMOTE_ROOT/script/make_install.sh > /dev/null
    fi
    ssh $MY_USERID@"$host" "chown -R root.$GRP $REMOTE_ROOT;chmod 755 $REMOTE_ROOT/script/make_install.sh"
done


# Call out to make_install.sh
echo "Beginning installation..."
for host in "${ALLNODES[@]}"; do
    [ "$host" = 'none' ] && continue;
    ssh $MY_USERID@"$host" "$REMOTE_ROOT/script/make_install.sh $SERVER $SERVER_ROOT $MEMSZ $GRP"
done

echo "Installation complete."
