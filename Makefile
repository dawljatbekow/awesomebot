# Target specific macros
TARGET = main
TARGET_SOURCES = $(addprefix src/, \
	main.c )
TOPPERS_OSEK_OIL_SOURCE = ./main.oil

NEXTTOOL_ROOT = /home/robo/bricxcc

# Don't modify below part
O_PATH ?= build

UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
	INC=/Applications/robolab/nxtOSEK/toppers_osek/kernel
	include /Applications/robolab/nxtOSEK/ecrobot/ecrobot.mak
else
	INC=/home/robo/nxtOSEK/nxtOSEK/toppers_osek/kernel
	include /home/robo/nxtOSEK/nxtOSEK/ecrobot/ecrobot.mak
endif



