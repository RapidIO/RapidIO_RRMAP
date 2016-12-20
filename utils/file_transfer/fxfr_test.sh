#!/bin/bash

MPORT=0;
SOURCE_PATH=/opt/rapidio/rapidio_sw
RIO_CLASS_MPORT_DIR=/sys/class/rio_mport/rio_mport$MPORT

MAXIT=1

RND=$RANDOM

. /etc/rapidio/nodelist.sh

MY_DESTID=$(cat $RIO_CLASS_MPORT_DIR/device/port_destid 2>/dev/null);

DID_LIST='';
DID_LIST_ALL='';
NODE_LIST='';
let nodec=0;
for node in $NODES; do
  RIODEVS=$(ssh root@"$node" "ls /sys/bus/rapidio/devices/")
  [ -z "$RIODEVS" ] && continue;
  DESTID=$(ssh root@"$node" "cat $RIO_CLASS_MPORT_DIR/device/port_destid 2>/dev/null")
  DID_LIST_ALL="$DID_LIST_ALL $DESTID";
  [ $MY_DESTID = $DESTID ] && continue;

  DID_LIST="$DID_LIST $DESTID";
  NODE_LIST="$NODE_LIST $node";
  let nodec=nodec+1;
done
echo "Valid node list: $NODE_LIST";
echo "Valid did  list: $DID_LIST";

# Startup fxfr_server on all nodes
screen -dmS fxfr_srv$RND 'bash'; sleep 0.2
screen -r fxfr_srv$RND -X stuff "cd $SOURCE_PATH/utils/file_transfer; sudo ./fxfr_server -m$MPORT -W$nodec -n | tee -a /tmp/fxfrsrv-${RND}.log^M"

for node in $NODE_LIST; do
  DID=$(printf "%d" $MY_DESTID);
  ssh root@"$node" "screen -dmS fxfr_srv$RND 'bash'; sleep 0.2; screen -r fxfr_srv$RND -X stuff \"cd $SOURCE_PATH/utils/file_transfer; ./fxfr_server -m$MPORT -W$nodec -n | tee -a /tmp/fxfrsrv-${RND}.log^M\""
done

# Startup rftp on all nodes, targetting all nodes but self

screen -dmS fxfr_cli$RND 'bash'; sleep 0.2
screen -r fxfr_cli$RND -X stuff "cd $SOURCE_PATH/utils/file_transfer; sudo ./fxfr_client.sh $MPORT $DID_LIST_ALL | tee -a /tmp/fxfrcli-${RND}.log^M"

for node in $NODE_LIST; do
  DID=$(printf "%d" $MY_DESTID);
  ssh root@"$node" "screen -dmS fxfr_cli$RND 'bash'; sleep 0.2; screen -r fxfr_cli$RND -X stuff \"cd $SOURCE_PATH/utils/file_transfer; ./fxfr_client.sh $MPORT $DID_LIST_ALL | tee -a /tmp/fxfrcli-${RND}.log^M\""
done


for ((i=0; i<$MAXIT; i++)); do
  echo "====Iteration $i====";

  echo "Server node:";
  cat /tmp/fxfrcli-${RND}.log; echo '' > /tmp/fxfrcli-${RND}.log;
  echo;

  for node in $NODE_LIST; do
    echo "Client node $node:";
    ssh root@"$node" "cat /tmp/fxfrcli-${RND}.log; echo '' > /tmp/fxfrcli-${RND}.log";
    echo;
  done
  sleep 10;
done

echo -n 'Press ENTER to tear down: '; read FOO

for node in $NODE_LIST; do
  ssh root@"$node" "screen -r fxfr_cli$RND -X stuff '^C^C'; sleep 0.5; screen -r fxfr_cli$RND -X stuff 'exit^M'; rm -f /tmp/fxfrcli-${RND}.log"
  ssh root@"$node" "screen -r fxfr_srv$RND -X stuff '^C^\^C'; sleep 0.5; screen -r fxfr_srv$RND -X stuff 'exit^M'; rm -f /tmp/fxfrsrv-${RND}.log"
done

screen -r fxfr_cli$RND -X stuff "^C^C"
screen -r fxfr_cli$RND -X stuff "exit^M"
screen -r fxfr_srv$RND -X stuff "^C^\^C"
sleep 0.5
screen -r fxfr_srv$RND -X stuff "exit^M"
rm -f /tmp/fxfrsrv-${RND}.log
