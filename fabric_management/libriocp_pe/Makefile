# Copyright (c) 2014, Prodrive Technologies
#
# SPDX-License-Identifier: BSD-3-Clause

TOPDIR?=$(shell pwd)/../..
include $(TOPDIR)/rules.mk

NAME:=riocp_pe
TARGETS:=lib$(NAME).a
TEST_TARGETS:=$(NAME)_test

OBJECTS:=$(patsubst src/%.c,src/%.o,$(wildcard src/*.c))
TEST_OBJECTS:=$(patsubst test/%.c,test/%.o,$(wildcard test/*.c))

CC=$(CXX) # XXX very UGGLY switcheroo
HEADERS:=inc/lib$(NAME).h

CFLAGS+=$(addprefix -W,all extra) -fPIC
CFLAGS+=-D$(DEBUG_CTL) -DRDMA_LL=$(LOG_LEVEL)
CFLAGS+=-I../inc
CFLAGS+=-I./inc -I./src
CFLAGS+=-I../librio_switch/inc -I../libct/inc -I../libdid/inc

LDFLAGS+=-L. -L$(COMMONLIB) -L$(FMDDIR)/libs_a $(LIBS_RPATH) -l$(NAME)


.PHONY: all clean

ifdef TEST
all: $(TARGETS) $(TEST_TARGETS)
else
all: $(TARGETS)
endif

%.a: $(OBJECTS)
	@echo ---------- Building $@
	$(AR) rcs $@ $^

src/%.o: src/%.c $(HEADERS)
	@echo ---------- Building $@
	$(CC) -c $(CFLAGS) $< -o $@ \
	$(TST_INCS)

test/%.o: test/%.c
	@echo ---------- Building $@
	$(CC) -c $(CFLAGS) $< -o $@ \
	$(TST_INCS)

$(TEST_TARGETS): $(TEST_OBJECTS)
	@echo ---------- Building $@
	$(CC) -o $@ $< $(LDFLAGS) \
	$(TST_LIBS) -lct -ldid -lcli

clean:
	@echo ---------- Cleaning lib$(NAME)...
	rm -f $(TARGETS) $(OBJECTS) \
	$(TEST_TARGETS) $(TEST_OBJECTS) \
	inc/*~ src/*~ test/*~ *~
