#!/bin/bash

#  This script stops all RIO cluster nodes.

SOURCE_PATH="/opt/rapidio/rapidio_sw"
INSTALL_PATH=$SOURCE_PATH"/install"

. /etc/rapidio/nodelist.sh

for node in $REVNODES
do
	echo "${node}"

	ssh root@"${node}" $INSTALL_PATH/kernel_stop.sh
done
