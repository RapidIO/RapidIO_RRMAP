#!/bin/bash

# Files required for installation
# Note the names of these file name (different root) are also used by make_install.sh
#
REMOTE_ROOT="/opt/rapidio/.install"
LOCAL_SOURCE_ROOT="$(pwd)"
SCRIPTS_PATH=$LOCAL_SOURCE_ROOT/install

NODEDATA_FILE="nodeData.txt"
TMP_NODEDATA_FILE=/tmp/$$_$NODEDATA_FILE
SRC_TAR="rapidio_sw.tar"
TMPL_FILE="config.tmpl"

MY_USERID=root

PGM_NAME=install_list.sh
PGM_NUM_PARMS=5

# Validate input
#
PRINTHELP=0

if [ "$#" -lt $PGM_NUM_PARMS ]; then
    echo $'\n$PGM_NAME requires $PGM_NUM_PARMS parameters.\n'
    PRINTHELP=1
else
    SERVER=$1

    OK=1
    ALLNODES=();
    # format of input file: <master|slave> <hostname> <rioname> <nodenumber>
    while read -r line || [[ -n "$line" ]]; do
        # allow empty lines
        if [ -z "$line" ]; then
            continue;
        fi

        arr=($line)
        if [ ${#arr[@]} -lt 4 ]; then
            echo "Incorrect line format ($line) in $2"
            OK=0
        fi

        host="${arr[1]}"
        if [ "${arr[0]}" = 'master' ]; then
            if [ -n "$MASTER" ]; then
                echo "Multiple master entries ($line) in $2"
                OK=0
            fi
            MASTER=$host
        fi
        ALLNODES+=("$host")
        echo $line >> $TMP_NODEDATA_FILE
    done < "$2"

    if [ -z "$MASTER" ]; then
        echo "No master entry in $2"
        OK=0
    fi

    if [ $OK -eq 0 ]; then
        echo "Errors in nodeData file $2, exiting..."
        rm -rf $TMP_NODEDATA_FILE &> /dev/null
        exit
    fi

    MEMSZ=$3
    SW_TYPE=$4
    GRP=$5
    REL=$6

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
    echo "$PGM_NAME <SERVER> <nData> <memsz> <sw> <group> <rel>"
    echo "<SERVER> Name of the node providing the files required by installation"
    echo "<nData>  The file describing the target nodes of the install"
    echo "         The file has the format:"
    echo "         <master|slave> <IP_Name> <RIO_name> <node>"
    echo "         Where:"
    echo "         <IP_name> : IP address or DNS name of the node"
    echo "         <RIO_name>: Fabric management node name."
    echo "         <node>    : String to replace in template file,"
    echo "                     of the form node#."
    echo "         EXAMPLE: master 10.64.15.199 gry37 node1"
    echo "         NOTE: Example nodeData.sh files are create by install.sh"
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
    rm -rf $TMP_NODEDATA_FILE &> /dev/null
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
    rm -rf $TMP_NODEDATA_FILE &> /dev/null
    exit
fi


echo "Creating install files for $SERVER..."
# First create the files that would be available on the server
#
TMP_DIR="/tmp/$$"
rm -rf $TMP_DIR;mkdir -p $TMP_DIR

# Copy nodeData.txt
#
mv $TMP_NODEDATA_FILE $TMP_DIR/$NODEDATA_FILE

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
