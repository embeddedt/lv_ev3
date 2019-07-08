SHELL:=/bin/bash
#
# Makefile
#
CC= arm-linux-gnueabi-gcc
CXX= arm-linux-gnueabi-g++
LVGL_DIR ?= ${shell pwd}
CFLAGS ?= -Wall -Wshadow -Wundef -Wmaybe-uninitialized -O3 -ggdb -I$(LVGL_DIR)/ -Iev3dev-c
CXXFLAGS ?=
LDFLAGS ?= -lpthread -L ev3dev-c-build -lev3dev
BIN = demo

export CC
export CXX


#Collect the files to compile
MAINSRC = ./main.c

include $(LVGL_DIR)/lvgl/lvgl.mk
include $(LVGL_DIR)/lv_drivers/lv_drivers.mk
include $(LVGL_DIR)/lv_examples/lv_examples.mk

OBJEXT ?= .o

AOBJS = $(ASRCS:.S=$(OBJEXT))
COBJS = $(CSRCS:.c=$(OBJEXT))

MAINOBJ = $(MAINSRC:.c=$(OBJEXT))

SRCS = $(ASRCS) $(CSRCS) $(MAINSRC)
OBJS = $(AOBJS) $(COBJS)

## MAINOBJ -> OBJFILES

.PHONY: all default clean

all: default

%.o: %.c
	@echo "CC $<"
	@$(CC)  $(CFLAGS) -c $< -o $@

%.o: %.cpp
	@echo "CXX $<"
	@$(CXX)  $(CXXFLAGS) $(CFLAGS) -c $< -o $@

ev3dev-c-build/Makefile:
	@if [ "$(shell echo $(MAKECMDGOALS) | grep -c clean)" -ne "1" ]; then cd ev3dev-c-build; cmake ../ev3dev-c -DEV3DEV_PLATFORM=EV3; fi

ev3dev_c: ev3dev-c-build/Makefile
	@if [ "$(shell echo $(MAKECMDGOALS) | grep -c clean)" -ne "1" ]; then $(MAKE) -C ev3dev-c-build; fi

default: $(AOBJS) $(COBJS) $(MAINOBJ)
	$(CXX) -o $(BIN) $(MAINOBJ) $(AOBJS) $(COBJS) $(LDFLAGS)
clean:
	rm -f $(BIN) $(AOBJS) $(COBJS) $(MAINOBJ)
	cd ev3dev-c-build; rm -r *; touch dir.txt

Makefile: ev3dev_c

