#Copyright (c) 2015, Integrated Device Technology Inc.
#Copyright (c) 2015, RapidIO Trade Association
#All rights reserved.
#
##Redistribution and use in source and binary forms, with or without modification,
#are permitted provided that the following conditions are met:
#
##1. Redistributions of source code must retain the above copyright notice, this
#list of conditions and the following disclaimer.
#
##2. Redistributions in binary form must reproduce the above copyright notice,
#this list of conditions and the following disclaimer in the documentation
#and/or other materials provided with the distribution.
#
##3. Neither the name of the copyright holder nor the names of its contributors
#may be used to endorse or promote products derived from this software without
#specific prior written permission.
#
##THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
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

TOPDIR?=$(shell pwd)/..
include $(TOPDIR)/rules.mk

DEBUG ?= DEBUG
export DEBUG

COMMON_DIR ?= $(COMMONDIR)
export COMMON_DIR

LOCAL_LIBRARY_DIR=libs_a

level ?= $(LOG_LEVEL)

export LOCAL_LIBRARY_DIR

CCFLAGS= $(CXXFLAGS) -fPIC -Wall -I. \
	-I./librio_switch/inc \
	-I./libriocp_pe/inc -I./librio_maint/inc \
	-I./libriocp_pe/src \
	-DRDMA_LL=$(level)

LIB_DEPS= libdid libct libriocp_pe libcli librio_switch libdd libcfg libpe_mpdrv

.PHONY: all clean daemon libfmdd $(LIB_DEPS)

all: daemon libfmdd

%.o: %.c $(CLI_DEPS) $(LIB_DEPS)
	$(CXX) -o $@ -c $< $(CCFLAGS)

libdid: FORCE
	$(MAKE) all -C libdid
	mkdir -p $(LOCAL_LIBRARY_DIR)
	cp libdid/libdid.a $(LOCAL_LIBRARY_DIR)

libct: FORCE
	$(MAKE) all -C libct
	mkdir -p $(LOCAL_LIBRARY_DIR)
	cp libct/libct.a $(LOCAL_LIBRARY_DIR)

libriocp_pe: FORCE
	$(MAKE) all -C libriocp_pe
	mkdir -p $(LOCAL_LIBRARY_DIR)
	cp libriocp_pe/libriocp_pe.a $(LOCAL_LIBRARY_DIR)

librio_switch: FORCE
	$(MAKE) all -C librio_switch
	mkdir -p $(LOCAL_LIBRARY_DIR)
	cp librio_switch/librio_switch.a $(LOCAL_LIBRARY_DIR)

libdd: FORCE
	$(MAKE) all -C libdd
	mkdir -p $(LOCAL_LIBRARY_DIR)
	cp libdd/libdd.a $(LOCAL_LIBRARY_DIR)
	mkdir -p $(COMMONLIBA)
	cp libdd/libdd.a $(COMMONLIBA)
	
libcfg: FORCE
	$(MAKE) all -C libcfg
	mkdir -p $(LOCAL_LIBRARY_DIR)
	cp libcfg/libcfg.a $(LOCAL_LIBRARY_DIR)
	
libpe_mpdrv: FORCE
	$(MAKE) all -C libpe_mpdrv
	mkdir -p $(LOCAL_LIBRARY_DIR)
	cp libpe_mpdrv/libpe_mpdrv.a $(LOCAL_LIBRARY_DIR)
	
libfmdd: FORCE
	$(MAKE) all -C libfmdd
	mkdir -p $(COMMONLIBA)
	cp libfmdd/libfmdd.a $(COMMONLIBA)
	

daemon: $(LIB_DEPS)
	$(MAKE) all -C daemon

FORCE:

clean:
	rm -f fmd *.o *~ inc/*~ *.exe core $(LOCAL_LIBRARY_DIR)/*.so \
		$(LOCAL_LIBRARY_DIR)/*.a
	$(MAKE) clean -C daemon
	$(MAKE) clean -C libcfg
	$(MAKE) clean -C libct
	$(MAKE) clean -C libdd
	$(MAKE) clean -C libdid
	$(MAKE) clean -C libfmdd
	$(MAKE) clean -C libpe_mpdrv
	$(MAKE) clean -C librio_switch
	$(MAKE) clean -C libriocp_pe
