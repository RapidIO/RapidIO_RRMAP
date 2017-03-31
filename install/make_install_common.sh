#!/bin/bash
# usage: ./make_install_common.sh <server> <rootPath> <memSize> <unixGroup>

# Setup passed in parameters
#
SERVER=$1
ROOT_PATH=$2
MEM_SIZE=$3
GRP=$4

# Files required by install, match those names in install.sh
#
NODEDATA_FILE="nodeData.txt"
SRC_TAR="rapidio_sw.tar"
TMPL_FILE="config.tmpl"

echo "Installing on $(hostname)..."

# Setup install location
#
INSTALL_ROOT="/opt/rapidio/.install"
SOURCE_PATH="/opt/rapidio/rapidio_sw"
CONFIG_PATH="/etc/rapidio"
SCRIPTS_PATH=$SOURCE_PATH"/install"


# Ensure clean installs
# (Do not create/clean $INSTALL_ROOT, that was done for us by install.sh)
#
rm -rf $SOURCE_PATH;mkdir -p $SOURCE_PATH
rm -rf $CONFIG_PATH;mkdir -p $CONFIG_PATH

# Create directories
#
mkdir -p /var/tmp/rapidio
chmod 777 /var/tmp/rapidio
chown -R root.$GRP /var/tmp/rapidio

# Get the source files from the central server
#
echo "Transferring install files from $SERVER..."
scp root@"$SERVER":$ROOT_PATH/* $INSTALL_ROOT/ > /dev/null
chown -R root.$GRP $INSTALL_ROOT


# Configuration files generated as part of the install
#
echo "Creating configuration files..."
CONFIG_FILE=$CONFIG_PATH/fmd.conf
NODELIST_FILE=$CONFIG_PATH/nodelist.sh
RSRV_CONF=$CONFIG_PATH/rsvd_phys_mem.conf

# Create the nodelist.sh file
# format of input file: <master|slave> <hostname> <rioname> <nodenumber>
#
NODELIST=''
REVNODELIST=''
while read -r line || [[ -n "$line" ]]; do
    arr=($line)
    if [ "${arr[0]}" = 'master' ]; then
        NODELIST="${arr[2]} $NODELIST"
        REVNODELIST="$REVNODELIST ${arr[2]}"
        if [ -n "$MASTER" ]; then
            echo "Multiple master entries ($line) in $NODEDATA_FILE, exiting ..."
            exit 1
        fi
        MASTER=${arr[1]}
    elif [ "${arr[0]}" = 'slave' ]; then
        NODELIST="$NODELIST ${arr[2]}";
        REVNODELIST="${arr[2]} $REVNODELIST"
    else
        echo "Invalid entry ($line)) in $NODEDATA_FILE, exiting ..."
        exit 1
    fi
done < "$INSTALL_ROOT/$NODEDATA_FILE"

if [ -z "$MASTER" ]; then
    echo "No master entry in $NODEDATA_FILE, exiting ..."
    exit 1
fi

cat > $NODELIST_FILE <<EOF
NODES=" $NODELIST "
REVNODES=" $REVNODELIST "
EOF


cd $SOURCE_PATH
result=$?
if [ $result -ne 0 ]
then
	echo cd failed, exiting...
	exit $result
fi

tar -xomf $INSTALL_ROOT/$SRC_TAR > /dev/null
result=$?
if [ $result -ne 0 ]
then
	echo tar failed, exiting...
	exit $result
fi

cp $SCRIPTS_PATH/rsvd_phys_mem.conf $RSRV_CONF
result=$?
if [ $result -ne 0 ]
then
	echo Copy failed, exiting...
	exit $result
fi

# Compile the source
#
echo "Compile sources..."
make -s clean
make -s all
result=$?
if [ $result -ne 0 ]
then
	echo Make of installed code failed, exiting...
	exit $result
fi

echo "Generate documentation..."
doxygen doxyconfig


# Display GIT status
#
echo "GIT status..."
git status


# Create scripts required for DMA TUN and others...
#
echo "Creating scripts..."
cd utils/goodput/scripts
./create_start_scripts.sh 0 234 0x200000000


# Remove old configuration file
#
rm -f $CONFIG_FILE
