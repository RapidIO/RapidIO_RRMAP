#!/bin/bash
# usage: ./make_install.sh <server> <rootPath> <memSize> <unixGroup>

# Common to both master and slave
#
INSTALL_ROOT="/opt/rapidio/.install"
. $INSTALL_ROOT/script/make_install_common.sh $1 $2 $3 $4


# Install scripts
#
echo "Installing start scripts..."
SCRIPT_FILES=( rio_start.sh stop_rio.sh all_start.sh stop_all.sh check_all.sh 
    rsock0_start.sh rsock0_stop.sh
    all_down.sh )

for f in "${SCRIPT_FILES[@]}"
do
    cp $SCRIPTS_PATH/$f $SOURCE_PATH/$f
done


# Install fmd configuration
#
echo "Installing fmd configuration..."
awk -vM=$MEM_SIZE '
    /MEMSZ/{gsub(/MEMSZ/, M);}
    {print}' $INSTALL_ROOT/$TMPL_FILE > $CONFIG_FILE


# Set ownership of files
#
cd $SOURCE_PATH/..
chown -R root.$GRP rapidio_sw

cd $CONFIG_PATH/..
chown -R root.$GRP rapidio
