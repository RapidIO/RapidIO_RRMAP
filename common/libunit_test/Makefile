#----------------------------------------------------------------------------*
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

TOPDIR?=$(shell pwd)/../..
include $(TOPDIR)/rules.mk

NAME:=unit_test
TARGETS:=lib$(NAME).a lib$(NAME).so
TEST_TARGET:=$(NAME)_test

OBJECTS:=$(patsubst src/%.c,src/%.o,$(wildcard src/*.c))
TEST_OBJECTS:=$(patsubst test/%.c,test/%.o,$(wildcard test/*.c))

CXXFLAGS+=-D$(DEBUG_CTL) -DRDMA_LL=$(LOG_LEVEL)
CXXFLAGS+=-I../../fabric_management/librio_switch/inc
LINK_FLAGS+= -rdynamic -shared -Wl,-soname,lib$(NAME).so.$(SOVER)
LDFLAGS+=-L. -L$(COMMONLIB) $(LIBS_RPATH) -l$(NAME)


.PHONY: all clean

ifdef TEST
all: $(TARGETS) $(TEST_TARGET)
else
all: $(TARGETS)
endif

%.a: $(OBJECTS)
	@echo ---------- Building $@
	$(AR) rcs $@ $^

%.so: $(OBJECTS)
	@echo ---------- Building $@
	$(CXX) $(CXXFLAGS) -o $@.$(SOVER) $(OBJECTS) $(LINK_FLAGS)
	@ln -s $@.$(SOVER) $@ &>/dev/null

src/%.o: src/%.c
	@echo ---------- Building $@
	$(CXX) -c $(CXXFLAGS) $< -o $@

test/%.o: test/%.c
	@echo ---------- Building $@
	$(CXX) -c $(CXXFLAGS) $< -o $@

$(TEST_TARGET): $(TEST_OBJECTS)
	@echo ---------- Building $@
	$(CXX) -o $@ $< $(LDFLAGS) \
	-llog -lcli -ltime_utils -pthread

clean:
	@echo ---------- Cleaning lib$(NAME)...
	rm -f $(TARGETS) lib$(NAME).so.$(SOVER) $(TEST_TARGET) $(OBJECTS) $(TEST_OBJECTS) inc/*~ src/*~ *~
