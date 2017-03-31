#!/bin/bash

. /etc/rapidio/nodelist.sh

for node in $NODES
do
	# Display node name
	echo "+++ UNINSTALL RapidIO stack on $node +++"

	ssh root@"$node" "cd /etc; rm -fr rapidio; cd /opt/rapidio && rm -fr rapidio_sw"
done
