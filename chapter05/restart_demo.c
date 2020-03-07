/******************************************************************************
  Title          : restart_demo.c
  Author         : Stewart Weiss
  Created on     : Spring 2011
  Description    : Waits for user input and a keyboard interrupt
  Purpose        : To demonstrate what happens when system calls are or are not
                   restarted when they get interrupted.
  Usage          : restart_demo
  Build with     : gcc -o restart_demo restart_demo.c
  Modifications  : April 2, 2012

  Notes          :
  To use this program:
    First run it and type no keyboard interrupts. It expects you to enter 4
    characters followed by Enter.  It will produce no output.

    Second, type less than 4 characters then Ctrl-C. It will output the message
    "Read call was interrupted." followed by the unaltered value XXXX.

    Third, type less than 4 characters then Ctrl-\. It will wait for you to type
    4 more characters, followed by Enter. It will output the message
    "Read call was interrupted and restarted." followed by the last 4
    characters that you typed.

 ******************************************************************************
 * Copyright (C) 2019 - Stewart Weiss
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.



******************************************************************************/



#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>


volatile sig_atomic_t  got_interrupt;

void on_interrupt(int dummy)
{
    got_interrupt = 1;
}

void die(char *s)
{
    fprintf(stderr, "%s\n", s);
    exit(1);
}

int main(int argc, char* argv[])
{
    struct sigaction sigact;
    int n;
    char c[5] = "XXXX";

    sigact.sa_handler = on_interrupt;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags = 0;
    if (sigaction(SIGINT, &sigact, NULL))
            die("sigaction-SIGINT");
    sigact.sa_flags = SA_RESTART;
    if (sigaction(SIGQUIT, &sigact, NULL))
            die("sigaction-SIGQUIT");

    got_interrupt = 0;

    /* Perform the read of 4 chars. */
    n = read(0, &c, 4);

    /* If a SIGINT interrupt was received SA_RESTART was off
       so the call will not be restarted. */
    if (-1 == n  &&  EINTR == errno ) {
            printf("Read call was interrupted.\n");
            write(1, c, 4);
            printf("\n");
    }
    /* If a SIGQUIT interrupt was received SA_RESTART was on
       so the call will be restarted. */
    else if (got_interrupt) {
            printf("Read call was interrupted and restarted.\n");
            write(1, c, 4);
            printf("\n");
    }

    /* Drain the terminal queues in case there are characters still there. */
    tcflush(0,TCIOFLUSH);
    return 0;
}

