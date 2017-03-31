# ----------------------------------------------------------------------------*
# Copyright (c) 2015, Integrated Device Technology Inc.
# Copyright (c) 2015, RapidIO Trade Association
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
# 
# 1. Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer.
# 
# 2. Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the documentation
# and/or other materials provided with the distribution.
# 
# 3. Neither the name of the copyright holder nor the names of its contributors
# may be used to endorse or promote products derived from this software without
# specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# --------------------------------------------------------------------------***
# Copyright (c) 2015, Integrated Device Technology
#

TOPDIR?=$(shell pwd)/../..

include $(TOPDIR)/rules.mk

export LOCAL_LIBRARY_DIR=libs_a
NAME=file_transfer

COMMON_LIBS_DIR?=../../common/libs_a

CXXFLAGS+=-D$(DEBUG_CTL) -DRDMA_LL=$(LOG_LEVEL)
CXXFLAGS+=-I. -I./file_xfer_lib/inc -I./libcdev/inc -I./libfxfr/inc
CXXFLAGS+=-I$(FMDDIR)/librio/inc

LDFLAGS_STATIC+=-L$(LOCAL_LIBRARY_DIR) -L$(COMMON_LIBS_DIR)
LDFLAGS_STATIC+=-lfxfr -lmport -lrsvdmem -llog -lcli
LDFLAGS_DYNAMIC+=-ldl -lrt

CLIENT_DEPS=
CLIENT_OBJS= client.o

SERVER_DEPS=
SERVER_OBJS= server.o

LIB_DEPS= libfxfr


.PHONY: $(LIB_DEPS)


%.o: %.c $(SERVER_DEPS) $(LIB_DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

libfxfr:
	$(MAKE) all -C libfxfr
	@mkdir -p $(LOCAL_LIBRARY_DIR) || true
	cp libfxfr/libfxfr.a $(LOCAL_LIBRARY_DIR)
	
fxfr_server: $(SERVER_OBJS) 
	$(CXX) $(CXXFLAGS) -o $@ $^ \
	$(LDFLAGS_STATIC) \
	$(LDFLAGS_DYNAMIC) \
	-D__CLI_LINUX__

rftp: $(CLIENT_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ \
	$(LDFLAGS_STATIC) \
	$(LDFLAGS_DYNAMIC) \
	-D__CLI_LINUX__ -I./libfxfr/inc

all: $(LIB_DEPS) libfxfr fxfr_server rftp

clean:
	@echo ---------- Cleaning $(NAME)...
	rm -f *.o *~ *.exe core $(LOCAL_LIBRARY_DIR)/*.a fxfr rftp fxfr_server libs_a/*
	$(MAKE) clean -C libfxfr
