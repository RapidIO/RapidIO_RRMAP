#!/bin/bash
cd $1
make -s clean
make -s all
doxygen doxyconfig
git status
cd utils/goodput/scripts
## Create scripts required for DMA TUN and others...
./create_start_scripts.sh 0 234 0x200000000
cd ../../../..
chgrp -R $2 rapidio_sw
