#!/bin/bash

MPORT=$1; shift;

[ -z "$MPORT" ] && { echo "Must specify MPORT!" 1>&2; exit 1; }

SOURCE_PATH=/opt/rapidio/rapidio_sw
RIO_CLASS_MPORT_DIR=/sys/class/rio_mport/rio_mport$MPORT

. /etc/rapidio/nodelist.sh

MY_DESTID=$(cat $RIO_CLASS_MPORT_DIR/device/port_destid 2>/dev/null);

DID_LIST=$@;
if [ -z "$DID_LIST" ]; then
  for node in $NODES; do
    RIODEVS=$(ssh root@"$node" "ls /sys/bus/rapidio/devices/")
    [ -z "$RIODEVS" ] && continue;
    DESTID=$(ssh root@"$node" "cat $RIO_CLASS_MPORT_DIR/device/port_destid 2>/dev/null")
    [ $MY_DESTID = $DESTID ] && continue;

    DID_LIST="$DID_LIST $DESTID";
  done
fi
echo "Valid did  list: $DID_LIST";

[ $(id -u) -gt 0 ] && { echo "Must run as root!" 1>&2; exit 1; }

mkdir /tmp/rd &>/dev/null
mount none /tmp/rd -t tmpfs;

let stop_req=0
trap "let stop_req=1; killall rftp; umount /tmp/rd" 0 1 2 15

src=/tmp/rd/1G;
dd if=/dev/zero of=$src bs=1M count=1024 &>/dev/null

dbg=-1
kbuf=1
while [ $stop_req -eq 0 ]; do
  for did in $DID_LIST; do
     [ $MY_DESTID = $did ] && continue;

     decdid=$(printf "%d" $did);
    ./rftp $src /dev/null $decdid 5555 $MPORT $dbg $kbuf
  done
done
