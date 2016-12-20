#!/bin/bash

trap "atexit" 0 1 2 15

declare -a JOBS;
let exitflag=0

function atexit()
{
  let n=${#JOBS[@]}
  for((i=0; i<n; i++)) do
    pid=${JOBS[$i]}
    kill -9 $pid &>/dev/null
  done
  killall ping &>/dev/null
  let exitflag=1
}

. /etc/rapidio/nodelist.sh

MY_IP=$( /sbin/ifconfig rsock0 | awk '/inet /{print $2}' );

PEERS=''
for node in $NODES
do
        # Check that the node was properly enumerated
        IP=$(ssh root@"$node" "/sbin/ifconfig rsock0" | awk '/inet /{print $2}')
	[ "$MY_IP" = "$IP" ] && continue;
	PEERS="$PEERS $IP"
done

if [ -f IPERFBARR ]; then
  echo Please remove IPERFBARR barrier file. The folder $PWD should be exported via sshfs too all nodes of cluster.
  while [ -f IPERFBARR ]; do
    sleep 0.1
    [ $exitflag -ne 0 ] && exit 0;
  done
fi

echo Barrier met. Blasting $PEERS

echo $@ | grep -q ping && {
  for p in $PEERS; do
    # Tsi721 has up to 16 reassembly contexts?
    for((i=0; i < 18; i++)); do
      sudo ping -qf $p &>/dev/null &
    done
  done
}

let c=0;
for p in $PEERS; do
  L=${HOSTNAME}-to-${p}.log
  rm $L &>/dev/null
  LP="-t 600"
  #LP="-t 3600"
  #LP="-t 43200"
  iperf -fMB $LP -c $p $@ &> $L &
  JOBS[$c]=$!
  let c=c+1
done

for p in $PEERS; do
  wait
done
