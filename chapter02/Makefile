# Makefile for Chapter 02
# This is a very simple Makefile, not taking much advantage of 
# built-in rules other than C source to C object.. 
# Type: 
#       make          to compile all the programs in the chapter 
#       make clean    to remove objects files and executables
#       make progname to make just progname

CC      =  /usr/bin/gcc
OBJS    =  *.o
EXECS   =  cp1 cp2 cp3 who1 who2 who3 who4 who_p show_utmp \
          show_utmp2 add_timerec2wtmp logout_utmp
OBJS      := $(patsubst %, %.o, $(EXECS))
SRCS      := $(patsubst %.o, %.c, $(OBJS))
CFLAGS  +=  -DSHOWHOST -Wall -g -I../include
LDFLAGS +=  -L../lib -lutils

.PHONY: all

all: $(EXECS)

.PHONY: all clean  cleanall
clean:
	-rm -f $(OBJS)

cleanall:
	-rm -f $(OBJS) $(EXECS)

$(EXECS): %: %.o
	$(CC) $(CFLAGS)  $< $(LDFLAGS) -o $@


who5:  who5.c utmp_utils.o  
	$(CC) $(CFLAGS) utmp_utils.o who5.c $(LDFLAGS) -o $@


