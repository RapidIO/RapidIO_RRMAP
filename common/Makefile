#----------------------------------------------------------------------------*
#Copyright (c) 2015, Integrated Device Technology Inc.
#Copyright (c) 2015, RapidIO Trade Association
#All rights reserved.
#
#Redistribution and use in source and binary forms, with or without modification,
#are permitted provided that the following conditions are met:
#
#1. Redistributions of source code must retain the above copyright notice, this
#list of conditions and the following disclaimer.
#
#2. Redistributions in binary form must reproduce the above copyright notice,
#this list of conditions and the following disclaimer in the documentation
#and/or other materials provided with the distribution.
#
#3. Neither the name of the copyright holder nor the names of its contributors
#may be used to endorse or promote products derived from this software without
#specific prior written permission.
#
#THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
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

all: libcmocka libset libcli liblist liblog libtime_utils \
	libmport libunit_test librsvdmem

libcmocka: FORCE
	$(MAKE) -C libcmocka
	mkdir -p libs_so
	cp libcmocka/libcmocka.so* libs_so/

libset: FORCE libcmocka
	$(MAKE) all -C libset
	mkdir -p libs_a
	mkdir -p libs_so
	cp libset/libset.a libs_a/
	cp libset/libset.so* libs_so/

libcli: FORCE libcmocka
	$(MAKE) all -C libcli
	mkdir -p libs_a
	mkdir -p libs_so
	cp libcli/libcli.a libs_a/
	cp libcli/libcli.so* libs_so/
		
liblist: FORCE libcmocka 
	$(MAKE) all -C liblist
	mkdir -p libs_a
	mkdir -p libs_so
	cp liblist/liblist.a libs_a/
	cp liblist/liblist.so* libs_so/
		
liblog: FORCE libcmocka 
	$(MAKE) all -C liblog
	mkdir -p libs_a
	mkdir -p libs_so
	cp liblog/liblog.a libs_a/
	cp liblog/liblog.so* libs_so/
		
libtime_utils: FORCE libcmocka liblog libcli
	$(MAKE) all -C libtime_utils
	mkdir -p libs_a
	mkdir -p libs_so
	cp libtime_utils/libtime_utils.a libs_a/
	cp libtime_utils/libtime_utils.so* libs_so/
		
libunit_test: FORCE libcmocka liblog libcli libtime_utils
	$(MAKE) all -C libunit_test
	mkdir -p libs_a
	mkdir -p libs_so
	cp libunit_test/libunit_test.a libs_a/
	cp libunit_test/libunit_test.so* libs_so/
		
librsvdmem: FORCE libcmocka libcli
	$(MAKE) all -C librsvdmem
	mkdir -p libs_a
	mkdir -p libs_so
	cp librsvdmem/librsvdmem.a libs_a/
	cp librsvdmem/librsvdmem.so* libs_so/
		
libmport: FORCE libcmocka libtime_utils
	$(MAKE) all -C libmport
	mkdir -p libs_a
	mkdir -p libs_so
	cp libmport/libmport.a libs_a/
	cp libmport/libmport.so* libs_so/

FORCE:

clean:
	$(MAKE) clean -C libcmocka
	$(MAKE) clean -C libset
	$(MAKE) clean -C libcli
	$(MAKE) clean -C liblist
	$(MAKE) clean -C liblog
	$(MAKE) clean -C libtime_utils
	$(MAKE) clean -C libunit_test
	$(MAKE) clean -C libmport
	$(MAKE) clean -C librsvdmem
	rm -f libs_a/*
	rm -f libs_so/*

