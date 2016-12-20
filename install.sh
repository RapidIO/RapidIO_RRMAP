#!/bin/bash

SOURCE_PATH="/opt/rapidio/rapidio_sw"
CONFIG_PATH="/etc/rapidio"
SCRIPTS_PATH=$SOURCE_PATH"/install"

PRINTHELP=0

if [ "$#" -lt 7 ]; then
	echo $'\ninstall.sh requires 7 parameters.\n'
	PRINTHELP=1
elif [ $5 != 'mem34' -a $5 != 'mem50' -a $5 != 'mem66' ] ; then
	echo $'\nmemsz parameter must be mem34, mem50, or mem66.\n'
	PRINTHELP=1
elif [ $6 != 'PD_tor' -a $6 != 'SB_re' -a $6 != '5G' -a $6 != '5G-PWR' -a $6 != 'AUTO' -a $6 != 'RXS' ] ; then
	echo $'\nsw parameter must be PD_tor or SB_re or 5G or 5G-PWR or AUTO\n'
	PRINTHELP=1
fi

if [ $PRINTHELP = 1 ] ; then
    echo "install.sh <NODE1> <NODE2> <NODE3> <NODE4> <memsz> <sw> <group> <rel>"
    echo "<NODE1> Name of master, enumerating node"
    echo "<NODE2> Name of slave node connected to Switch Port 2"
    echo "<NODE3> Name of slave node connected to Switch Port 3"
    echo "<NODE4> Name of slave node connected to Switch Port 4"
    echo "If any of <NODE2> <NODE3> <NODE4> is \"none\", the node is ignored."
    echo "<memsz> RapidIO memory size, one of mem34, mem50, mem66"
    echo "        If any node has more than 8 GB of memory, MUST use mem50"
    echo "<sw>    Type of switch the four nodes are connected to."
    echo "        PD_tor - Prodrive Technologies Top of Rack Switch"
    echo "        SB_re  - StarBridge Inc RapidExpress Switch"
    echo "        5G     - 5G lab configuration"
    echo "        5G-PWR - 5G lab configuration for Power-8 nodes"
    echo "        AUTO   - configuration determined at runtime"
    echo "        RXS    - RXS configuration"
    echo "<group> Unix file ownership group which should have access to"
    echo "        the RapidIO software"
    echo "<rel> is the software release/version to install."
    echo "        If no release is supplied, the current release is installed."
    exit
fi

MASTER=$1
SLAVES=( )
ALLNODES=( $1 )
NODE2=""
NODE3=""
NODE4=""

if [ $2 != 'none' ]; then
	NODE2=$2
	SLAVES[1]=$2
	ALLNODES[1]=$2
fi

if [ $3 != 'none' ]; then
	NODE3=$3
	SLAVES[2]=$3
	ALLNODES[2]=$3
fi

if [ $4 != 'none' ]; then
	NODE4=$4
	SLAVES[3]=$4
	ALLNODES[3]=$4
fi

MEMSZ=$5
SW_TYPE=$6
GRP=$7
REL=$8

for i in "${ALLNODES[@]}"
do
	[ "$host" = 'none' ] && continue;
	ping -c 1 $i > /dev/null
	if [ $? -ne 0 ]; then
		echo $i " Not accessible, aborting..."
		exit
	else
		echo $i "accessible."
	fi
done

echo "Beginning installation..."

make clean &>/dev/null;

for host in  "${ALLNODES[@]}"; do
  [ "$host" = 'none' ] && continue;
  tar cf - * .git* | \
  ssh -C root@"$host" "rm -rf $SOURCE_PATH;mkdir -p $SOURCE_PATH; pushd $SOURCE_PATH &>/dev/null; tar xf -; popd &>/dev/null; chown -R root.$GRP $SOURCE_PATH"
done

for host in  "${ALLNODES[@]}"; do
  [ "$host" = 'none' ] && continue;
  ssh root@"$host" "$SCRIPTS_PATH/make_install.sh $SOURCE_PATH $GRP"
done

echo "Installing configuration files..."

if [ "$SW_TYPE" = 'AUTO' ]; then
	MASTER_CONFIG_FILE=install/auto-master.conf
fi
if [ "$SW_TYPE" = 'SB_re' ]; then
	MASTER_CONFIG_FILE=install/node-master.conf
fi

if [ "$SW_TYPE" = 'PD_tor' ]; then
	MASTER_CONFIG_FILE=install/tor-master.conf
fi

if [ "$SW_TYPE" = '5G' ]; then
	MASTER_CONFIG_FILE=install/5g-master.conf
fi

if [ "$SW_TYPE" = '5G-PWR' ]; then
	MASTER_CONFIG_FILE=install/5g-pwr-master.conf
fi

if [ "$SW_TYPE" = 'RXS' ]; then
        MASTER_CONFIG_FILE=install/rxs-master.conf
fi

destids=($(grep ENDPOINT $MASTER_CONFIG_FILE | grep PORT | awk '{print $12}'))
comptags=($(grep ENDPOINT $MASTER_CONFIG_FILE | grep PORT | awk '{print $5}'))

FILENAME=$CONFIG_PATH/fmd.conf
MASTDEST=${destids[0]}

# FMD slaves go first.
# Slaves do not need a configuration file - make sure it is gone.
for c in $(seq 1 3); do
  host=${ALLNODES[c]};
  [ "$host" = 'none' ] && continue;
  [ -z "$host" ] && continue;
  ssh root@"$host" "rm -f $FILENAME";
done

HOSTL='';
let c=0;
for host in  "${ALLNODES[@]}"; do
  let c=c+1;
  # We allow none for sake of awk substitution
  HOSTL="$HOSTL -vH$c=$host";
done

# And now the master FMD
awk -vM=$MEMSZ $HOSTL '
	/MEMSZ/{gsub(/MEMSZ/, M);}
	/node1/{gsub(/node1/, H1);}
	/node2/{if(H2 != "") {gsub(/node2/, H2);} else {$0="";}}
	/node3/{if(H3 != "") {gsub(/node3/, H3);} else {$0="";}}
	/node4/{if(H4 != "") {gsub(/node4/, H4);} else {$0="";}}
	/node5/{if(H5 != "") {gsub(/node5/, H5);} else {$0="";}}
	/node6/{if(H6 != "") {gsub(/node6/, H6);} else {$0="";}}
	/node7/{if(H7 != "") {gsub(/node7/, H7);} else {$0="";}}
	/node8/{if(H8 != "") {gsub(/node8/, H8);} else {$0="";}}
	{print}' $MASTER_CONFIG_FILE | \
    ssh root@"$MASTER" "mkdir -p $CONFIG_PATH; cd $CONFIG_PATH; cat > $FILENAME";


RSRV_CONF=$CONFIG_PATH/rsvd_phys_mem.conf
for host in  "${ALLNODES[@]}"; do
  [ "$host" = 'none' ] && continue;
  ssh root@"$host" "mkdir -p $CONFIG_PATH; cp $SCRIPTS_PATH/rsvd_phys_mem.conf $RSRV_CONF"
done

echo "Installation of configuration files COMPLETED..."

FILES=( rio_start.sh stop_rio.sh all_start.sh stop_all.sh check_all.sh 
	rsock0_start.sh rsock0_stop.sh
	all_down.sh )

for f in "${FILES[@]}"
do
	FILENAME=$SOURCE_PATH/$f
	ssh root@"$MASTER" "cp $SCRIPTS_PATH/$f $FILENAME"
done

echo "Installing node list..."

NODELIST='NODES="'
REVNODELIST=" \"";
for host in  "${ALLNODES[@]}"; do
  [ "$host" = 'none' ] && continue;
  NODELIST="$NODELIST $host";
  REVNODELIST="$host $REVNODELIST";
done
NODELIST="$NODELIST \"";
REVNODELIST='REVNODES='"\" ${REVNODELIST}"
cat > /tmp/nodelist.sh <<EOF
$NODELIST
$REVNODELIST
EOF

for host in  "${ALLNODES[@]}"; do
  [ "$host" = 'none' ] && continue;
  (cd /tmp; tar cf - nodelist.sh) |
    ssh root@"$host" "mkdir -p $CONFIG_PATH; cd $CONFIG_PATH; tar xvf -";
done

rm /tmp/nodelist.sh

exit

echo "Installion complete."
