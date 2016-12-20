#----------------------------------------------------------------------------*
#Copyright (c) 2015, Integrated Device Technology Inc.
#Copyright (c) 2015, RapidIO Trade Association
#All rights reserved.
#
###Redistribution and use in source and binary forms, with or without modification,
#are permitted provided that the following conditions are met:
#
###1. Redistributions of source code must retain the above copyright notice, this
#list of conditions and the following disclaimer.
#
###2. Redistributions in binary form must reproduce the above copyright notice,
#this list of conditions and the following disclaimer in the documentation
#and/or other materials provided with the distribution.
#
###3. Neither the name of the copyright holder nor the names of its contributors
#may be used to endorse or promote products derived from this software without
#specific prior written permission.
#
###THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
#ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
#WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
#DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
#FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
#DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
#SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
#CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
#OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
#OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#--------------------------------------------------------------------------***
#

TOPDIR?=$(shell pwd)/../..
include $(TOPDIR)/rules.mk


LOG_LEVEL ?= 1

LOCAL_LIBRARY_DIR ?= libs_a

LIB_DIRS?= -L$(COMMONLIB) -L../$(LOCAL_LIBRARY_DIR) $(LIBS_RPATH)

OBJECTS:=$(patsubst src/%.c,src/%.o,$(wildcard src/*.c))

CXXFLAGS+=-I$(KDIR) -I$(RIODIR)
CXXFLAGS+=-I$(FMDDIR)/inc

src/%.o: src/%.c
	$(CXX) $(CXXFLAGS) -o $@ $< -c \
	-I../libdid/inc \
	-I../libct/inc \
	-I../librio_switch/inc \
	-I../libriocp_pe/inc \
	-I../libpe_mpdrv/inc \
	-I../libcfg/inc \
	-I../libmport/inc \
	-D$(DEBUG_CTL) -DRDMA_LL=$(LOG_LEVEL)

all: fmd

# The Fabric Management Daemon
fmd:  $(OBJECTS) 
	@echo ---------- Building fmd...
	$(CXX) -pthread \
	-o $@ $(OBJECTS) $(LIB_DIRS) \
	-lcfg -lct -ldid -lpe_mpdrv -lswitch_api -lriocp_pe \
	-ldd -llist -ltime_utils -lcli -llog -lmport \
	-lrt -ldl $(TST_LIBS)

clean:
	rm -f $(OBJECTS) inc/*~ src/*~ *~ fmd
