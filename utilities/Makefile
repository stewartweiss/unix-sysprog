#
# Makefile for Utilities
# Revised March 1, 2011
# Type  make          to build the library and header file 
#       make clean    to remove objects files and executables
#       make install  to copy the library into ../lib and the
#                     header to ../include

INSTALLDIR = ../lib
INCLUDEDIR = ../include
CC         = /usr/bin/gcc
AR         = /usr/bin/ar
SRCS       = $(wildcard *.c)
OBJS       = $(patsubst %.c,%.o,$(SRCS))

.PHONY: all clean cleanall install
all: libutils.a

install: libutils.a
	cp libutils.a $(INSTALLDIR)
	cat *.h > $(INCLUDEDIR)/utils.h

.c.o:
	$(CC) -g -c -fPIC  $< 

clean:
	\rm $(OBJS)

libutils.a: $(OBJS)
	$(AR) rcs libutils.a  $(OBJS)


