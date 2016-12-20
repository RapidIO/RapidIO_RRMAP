#!/bin/bash
cd $1
make -s clean
make -s all
doxygen doxyconfig
/bin/ldconfig $1/common/libs_so
git status
cd utils/goodput/scripts
## Create scripts required for DMA TUN and others...
./create_start_scripts.sh 0 234 200000000
cd ../../../..
chgrp -R $2 rapidio_sw
