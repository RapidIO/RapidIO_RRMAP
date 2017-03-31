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

NAME=fmd
TARGET=$(NAME)

OBJECTS:=$(patsubst src/%.c,src/%.o,$(wildcard src/*.c))

LOG_LEVEL?= 1

CXXFLAGS+=-D$(DEBUG_CTL) -DRDMA_LL=$(LOG_LEVEL)
CXXFLAGS+=-I$(KDIR) -I$(RIODIR)
CXXFLAGS+=-I$(FMDDIR)/inc
CXXFLAGS+=-I../libdid/inc
CXXFLAGS+=-I../libct/inc
CXXFLAGS+=-I../librio/inc
CXXFLAGS+=-I../libriocp_pe/inc
CXXFLAGS+=-I../libpe_mpdrv/inc
CXXFLAGS+=-I../libcfg/inc

LDFLAGS_STATIC+=-L$(COMMONLIB) -L../libs_a
LDFLAGS_STATIC+=-lriocp_pe -lpe_mpdrv -lrio -lcfg -ldid -lct
LDFLAGS_STATIC+=-ldd -llist -ltime_utils -lcli -llog -lmport
LDFLAGS_STATIC+=$(TST_LIBS)
LDFLAGS_DYNAMIC+=-lrt -ldl


.PHONY: all clean


all: $(TARGET)

src/%.o: src/%.c
	@echo ---------- Building $@
	$(CXX) $(CXXFLAGS) -o $@ $< -c

# The Fabric Management Daemon
$(TARGET): $(OBJECTS)
	@echo ---------- Building $@
	$(CXX) -o $@ $(OBJECTS) \
	-pthread \
	$(LDFLAGS_STATIC) \
	$(LDFLAGS_DYNAMIC)

clean:
	@echo ---------- Cleaning $(NAME)...
	rm -f $(TARGET) $(OBJECTS) \
	inc/*~ src/*~ test/*~ *~
