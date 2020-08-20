# Makefile for Chapter 01
# Written by Stewart Weiss
# usage:
#       make             to compile all the programs in the chapter
#       make cleanall    to remove objects files and executables
#       make clean       to remove only object files
#       make <progname>  to make just <progname>

# Note: This makefile uses implicit rules to build the .o files
#       for no particular reason other than to demonstrate how these rules
#       can be used. For example, the rule to create C object files is:
#       x.o is made automatically from x.c with a command of the form
#                  `$(CC) -c $(CPPFLAGS) $(CFLAGS)'.

CC      = /usr/bin/gcc
CXX     = /usr/bin/g++
MOREPROGS := more_v1 more_v2 more_v3
OTHERS    := showpid simple_C_io escapes systemcalldemo
EXECS     := $(MOREPROGS) $(OTHERS) simple_C++_io
OBJS      := $(wildcard *.o)
CFLAGS    += -Wall -g

all:    $(EXECS)
.PHONY: all clean cleanall

clean:
	$(RM) $(OBJS)

cleanall:
	$(RM) $(OBJS) $(EXECS)


$(OTHERS): %: %.o
	$(CC) $(CFLAGS) -o $@ $<

simple_C++_io:   simple_C++_io.o
	$(CXX) -o simple_C++_io  simple_C++_io.o

more_v1: more.o more_utils_v1.o more_utils.h
	$(CC) $(CFLAGS) -o more_v1 more.o more_utils_v1.o

more_v2: more.o more_utils_v2.o more_utils.h
	$(CC) $(CFLAGS) -o more_v2 more.o more_utils_v2.o

more_v3: more.o more_utils_v3.o more_utils.h
	$(CC) $(CFLAGS) -o more_v3 more.o more_utils_v3.o




