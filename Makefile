OUTPUT=libstring_util.a
SOURCES_EXT=.c
SOURCES_EXT2=.cpp
SOURCE_DIR=src/
CXXFLAGS=-g -Wall -I$(CURDIR)/include
# -gdwarf-2 -g3
LDFLAGS=
LDLIBS=
LIBDIRS=
#POSTCMDS=make create_include
HEADERS=string_util.h exceptions.h orderedmap
MAKEFILE_DIR=.

default: linux

all: linux qnx qnx65 bbb vxworks6.7-x86-gcc4.x vxworks6.8-x86-gcc4.x vxworks6.9-x86-gcc4.x win32

cppcheck:
	cppcheck --max-configs=50 -q -I include src
clean:
	-rm -rf obj

include $(MAKEFILE_DIR)/Makefile.archs

