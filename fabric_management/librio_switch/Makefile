# Copyright (c) 2014, Prodrive Technologies
# Copyright (c) 2014, Integrated Device Technology
#
# SPDX-License-Identifier: BSD-3-Clause

TOPDIR?=$(shell pwd)/../..
include $(TOPDIR)/rules.mk

NAME:=switch_api
HEADERS:=inc
TARGETS:=lib$(NAME).a
TEST_TARGET:=$(NAME)_test

LOG_LEVEL?=4
DEBUG?=DEBUG

CFLAGS+=$(addprefix -W,all extra) -fPIC
CFLAGS+=-DNDEBUG -DIDT_TSI721_SUPPORT
CFLAGS+=-I./inc
CFLAGS+=-I$(FMDDIR)/librio_switch/inc

LDFLAGS+=-Wl,--wrap=DARRegRead -Wl,--wrap=DARRegWrite
LDFLAGS+=-L. -L$(COMMONLIB) -L$(FMDDIR)/libs_a $(LIBS_RPATH) -l$(NAME)
LDFLAGS+=-lcmocka


OBJECTS:=$(patsubst src/%.c,src/%.o,$(wildcard src/*.c)) 
TEST_OBJECTS:=$(patsubst test/%.c,test/%.o,$(wildcard test/*.c))
TEST_TARGETS:=$(patsubst test/%.c,test/%,$(wildcard test/*.c))

.PHONY: all clean test


ifdef TEST
all: $(TARGETS) $(TEST_TARGET)
else
all: $(TARGETS)
endif

test: $(TEST_TARGET)

%.a: $(OBJECTS)
	@echo ---------- Building $@
	$(AR) rcs $@ $^

src/%.o: src/%.c
	@echo ---------- Building $@
	$(CXX) -c $(CFLAGS) $< -o $@ \
        $(TST_INCS)

test/%.o: test/%.c
	@echo ---------- Building $@
	$(CXX) -c $(CFLAGS) $(TST_INCS) $< -o $@
	

test/%: test/%.o
	@echo ---------- Building $@
	$(CXX) -o $@ $< $(LDFLAGS) \
	$(TST_LIBS)

$(TEST_TARGET): $(TEST_TARGETS)

clean:
	rm -f $(TARGETS) $(OBJECTS) \
        $(TEST_TARGETS) $(TEST_OBJECTS) \
	inc/*~ src/*~ test/*~ *~


