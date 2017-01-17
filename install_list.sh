#!/bin/bash

# Files required for installation
# Note the names of these file name (different root) are also used by make_install.sh
#
NODEDATA_FILE="nodeData.txt"
SRC_TAR="rapidio_sw.tar"
TMPL_FILE="config.tmpl"


# Validate input
#
PRINTHELP=0

if [ "$#" -lt 5 ]; then
    echo $'\ninstall_list.sh requires 5 parameters.\n'
    PRINTHELP=1
else
    SERVER=$1

    ALLNODES=();
    # format of input file: <master|slave> <hostname> <rioname> <nodenumber>
    while read -r line || [[ -n "$line" ]]; do
        arr=($line)
        host="${arr[1]}"
        if [ "${arr[0]}" = 'master' ]; then
            if [ -n "$MASTER" ]; then
                echo "Multiple master entries ($line) in $2, exiting ..."
                exit
            fi
            MASTER=$host
        fi
        ALLNODES+=("$host")
    done < "$2"

    if [ -z "$MASTER" ]; then
        echo "No master entry in $2, exiting ..."
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

    if [ $SW_TYPE != 'PD_tor' -a $SW_TYPE != 'SB_re' -a $SW_TYPE != 'AUTO' -a $SW_TYPE != 'RXS' ] ; then
        echo $'\nsw parameter must be PD_tor, SB_re, AUTO or RXS.\n'
        PRINTHELP=1
    fi
fi

if [ $PRINTHELP = 1 ] ; then
    echo "install_list.sh <SERVER> <nData> <memsz> <sw> <group> <rel>"
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
    echo "         NOTE: master must always be node1"
    echo "<memsz>  RapidIO memory size, one of mem34, mem50, mem66"
    echo "         If any node has more than 8 GB of memory, MUST use mem50"
    echo "<sw>     Type of switch the four nodes are connected to."
    echo "         PD_tor - Prodrive Technologies Top of Rack Switch"
    echo "         SB_re  - StarBridge Inc RapidExpress Switch"
    echo "         AUTO   - configuration determined at runtime"
    echo "         RXS    - RXS configuration"
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
ping -c 1 $SERVER > /dev/null
if [ $? -ne 0 ]; then
    echo "    $SERVER not accessible, exiting..."
    exit
else
    echo "    $SERVER accessible."
fi

for host in "${ALLNODES[@]}"
do
    [ "$host" = 'none' ] && continue;
    [ "$host" = "$SERVER" ] && continue;
    ping -c 1 $host > /dev/null
    if [ $? -ne 0 ]; then
        echo "    $host not accessible, exiting..."
        exit
    else
        echo "    $host accessible."
    fi
done


echo "Creating install files for $SERVER..."
# First create the files that would be available on the server
#
ROOT="/tmp/$$"
rm -rf $ROOT;mkdir -p $ROOT

# Copy nodeData.txt
#
cp $2 $ROOT/$NODEDATA_FILE

# Create the source.tar
#
make clean &>/dev/null
tar -cf $ROOT/$SRC_TAR * .git* &>/dev/null

# Copy the template file
#
if [ "$SW_TYPE" = 'AUTO' ]; then
    MASTER_CONFIG_FILE=auto-master.conf
    MASTER_MAKE_FILE=auto-master-make.sh
elif [ "$SW_TYPE" = 'SB_re' ]; then
    MASTER_CONFIG_FILE=node-master.conf
    MASTER_MAKE_FILE=node-master-make.sh
elif [ "$SW_TYPE" = 'PD_tor' ]; then
    MASTER_CONFIG_FILE=tor-master.conf
    MASTER_MAKE_FILE=tor-master-make.sh
elif [ "$SW_TYPE" = 'RXS' ]; then
    MASTER_CONFIG_FILE=rxs-master.conf
    MASTER_MAKE_FILE=rxs-master-make.sh
fi
cp install/$MASTER_CONFIG_FILE $ROOT/$TMPL_FILE

# Transfer the files to the server
#
echo "Transferring install files to $SERVER..."
SERVER_ROOT="/opt/rapidio/.server"
ssh root@"$SERVER" "rm -rf $SERVER_ROOT;mkdir -p $SERVER_ROOT"
scp $ROOT/* root@"$SERVER":$SERVER_ROOT/. > /dev/null
ssh root@"$SERVER" "chown -R root.$GRP $SERVER_ROOT"
rm -rf $ROOT

# Transfer the make_install.sh script to a known location on the target machines
#
INSTALL_ROOT="/opt/rapidio/.install"
SCRIPTS_PATH="$(pwd)"/install
for host in "${ALLNODES[@]}"; do
    [ "$host" = 'none' ] && continue;
    echo "Transferring install script to $host..."
    ssh root@"$host" "rm -rf $INSTALL_ROOT;mkdir -p $INSTALL_ROOT/script"
    scp $SCRIPTS_PATH/make_install_common.sh root@"$host":$INSTALL_ROOT/script/make_install_common.sh > /dev/null
    if [ "$host" = "$MASTER" ]; then
        scp $SCRIPTS_PATH/$MASTER_MAKE_FILE root@"$host":$INSTALL_ROOT/script/make_install.sh > /dev/null
    else
        scp $SCRIPTS_PATH/make_install-slave.sh root@"$host":$INSTALL_ROOT/script/make_install.sh > /dev/null
    fi
    ssh root@"$host" "chown -R root.$GRP $INSTALL_ROOT;chmod 755 $INSTALL_ROOT/script/make_install.sh"
done


# Call out to make_install.sh
echo "Beginning installation..."
for host in  "${ALLNODES[@]}"; do
    [ "$host" = 'none' ] && continue;
    ssh root@"$host" "$INSTALL_ROOT/script/make_install.sh $SERVER $SERVER_ROOT $MEMSZ $GRP"
done

echo "Installation complete."
