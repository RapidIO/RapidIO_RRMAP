# Copyright (c) 2014, Prodrive Technologies
# Copyright (c) 2014, Integrated Device Technology
#
# SPDX-License-Identifier: BSD-3-Clause

TOPDIR?=$(shell pwd)/../..
include $(TOPDIR)/rules.mk

NAME:=rio_switch
TARGETS:=lib$(NAME).a
TEST_TARGET:=$(NAME)_test

OBJECTS:=$(patsubst src/%.c,src/%.o,$(wildcard src/*.c))
TEST_OBJECTS:=$(patsubst test/%.c,test/%.o,$(wildcard test/*.c))
TEST_TARGETS:=$(patsubst test/%.c,test/%,$(wildcard test/*.c))

HEADERS:=inc
LOG_LEVEL?=4
DEBUG?=DEBUG

CFLAGS+=$(addprefix -W,all extra) -fPIC
CFLAGS+=-DNDEBUG -DIDT_TSI721_SUPPORT
CFLAGS+=-I./inc
CFLAGS+=-I$(FMDDIR)/librio_switch/inc

LDFLAGS+=-Wl,--wrap=DARRegRead -Wl,--wrap=DARRegWrite
LDFLAGS+=-L. -L$(COMMONLIB) -L$(FMDDIR)/libs_a $(LIBS_RPATH) -l$(NAME)

TST_LIBS+=-lcli

.PHONY: all clean

ifdef TEST
all: $(TARGETS) $(TEST_TARGET)
else
all: $(TARGETS)
endif

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
	@echo ---------- Cleaning lib$(NAME)...
	rm -f $(TARGETS) $(OBJECTS) \
	 $(TEST_TARGETS) $(TEST_OBJECTS) \
	inc/*~ src/*~ test/*~ *~

