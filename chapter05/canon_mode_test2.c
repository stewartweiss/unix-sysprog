/******************************************************************************
  Title          : canon_mode_test2.c
  Author         : Stewart Weiss
  Created on     : November 4, 2006
  Description    : Allows terminal settings to be controlled to see effects
  Purpose        :  Allows user to control O_NDELAY, ICANON, VMIN, VTIME
                    independently in order to see the effect on interactive
                    reads from the terminal, and handles signals crudely
  Usage          :
                   canon_mode_test2 [-hbc] [ -m MIN ] [ -t TIME ]
                    -h : help
                    -b : turn on blocking mode  (default is off)
                    -c : turn on canonical mode (default is off)
                    -m : set VMIN value
                    -t : set VTIME value
  Build with     : gcc -Wall -g -o canon_mode_test2 canon_mode_test2.c
  Modifications  : March 21, 2011
                   Cleaned up program a bit

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
#include <stdlib.h>
#include <termios.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#define  SLEEPTIME   2

/**
   gets  input under the mode specified by the values of iscanon, vmin, and
   vtime. See the notes or the termios man page for the meaning of the vmin
   and vtime parameters.
 */
void get_response( int iscanon, int vmin, int vtime);

/* sets the VMIN value to vmin, VTIME to vtime, turns off icanon */
void set_non_canonical(int vmin, int vtime);

/* Sets file descriptor fd to non-blocking mode */
void set_non_blocking_mode(int fd);

/* if how == 0 this saves the termios state for later restoring     */
/* if how == 1 this restores the termios state from the saved state */
/* CANNOT CALL with how == 1 before first calling with how == 0     */
void tty_mode(int how);

/* SIGINT signal handler -- restores tty state and exits */
void ctrl_c_handler(int signum);


int main(int argc, char* argv[])
{
    int    bNoCanon  = 1; /* default is turn on non-canonical mode */
    int    bNonBlock = 1; /* default is turn on O_NONBLOCK */
    int    vmin      = 1; /* default is one char */
    int    vtime     = 0; /* default is to force reads to wait for vmin chars */
    int    ch;
    char   optstring[] = ":hbcm:t:";

    while (1) {
        ch = getopt(argc, argv, optstring);
        if ( -1 == ch )
            break;
        switch (ch) {
        case 'h' :
            printf("Usage: %s [-hbc] [ -m MIN ] [ -t TIME ]\n",argv[0]);
            printf("       -h : help\n");
            printf("       -b : turn on blocking mode\n");
            printf("       -c : turn on canonical mode\n");
            printf("       -m : set VMIN value \n");
            printf("       -t : set VTIME value \n");
            exit(0);
        case 'b' :
            bNonBlock = 0;   /* meaning blocking is ON */
            break;
        case 'c' :
            bNoCanon = 0;    /* meaning canonical mode is ON */
            break;
        case 'm' :
            vmin = strtol(optarg,'\0',0);
            break;
        case 't' :
            vtime = strtol(optarg, '\0',0);
            break;
        case '?' :
            break;
        case ':' :
            break;
        default:
           fprintf(stderr, " ");
        }
    }

    /* install Ctrl-C signal handler */
    signal( SIGINT, ctrl_c_handler );

    /* ignore Ctrl-\ SIGQUIT signals */
    signal( SIGQUIT, SIG_IGN );


    /* Report on state that we will put terminal in */
    if ( bNoCanon )
        printf("Canonical mode off\n");
    else
        printf("Canonical mode on\n");

    if ( bNonBlock )
        printf("Non-blocking mode is on\n");
    else
        printf("Non-blocking mode is off\n");

    printf("VMIN set to %d\n",vmin);
    printf("VTIME set to %d\n",vtime);

    /* save current terminal state */
    tty_mode(0);

    /* if canonical mode flag, unset icanon */
    if ( bNoCanon )
         set_non_canonical(vmin, vtime);

    /* if non-blocking mode flag, turn on O_NDELAY */
    if ( bNonBlock )
        set_non_blocking_mode(STDIN_FILENO);

    /* call function to get input */
    get_response(!bNoCanon, vmin, vtime);

    /* restore terminal state */
    tty_mode(1);

    /* flush input stream */
    tcflush(0,TCIFLUSH);
    return 0;
}

void get_response( int iscanon, int vmin, int vtime)
{
    int    num_bytes_read;
    char   input[128];

    fflush(stdout);
    sleep(SLEEPTIME);
    if ( iscanon )
        printf("About to call read() in canonical mode\n");
    else
        printf("About to call read with MIN = %d and TIME = %d\n", vmin, vtime);
    sleep(1);
    printf("Enter some characters or wait to see what happens.\n");
    num_bytes_read = read(0, input, 10) ;
    if ( num_bytes_read >= 0 ) {
        input[num_bytes_read] = '\0';
        if ( num_bytes_read > 0 )
            printf("\nReturn value of read() is %d; chars read are %s\n",
                num_bytes_read, input );
        else
            printf("\nReturn value of read() is %d;"
                    " no chars were read\n",num_bytes_read);
    }
    else
        printf("read() returned -1\n");
}


void set_non_canonical(int vmin, int vtime)
{
    struct termios ttystate;

    tcgetattr( 0, &ttystate);            /*  read curr. setting    */
    ttystate.c_lflag        &= ~ICANON;  /*  no buffering          */
    ttystate.c_cc[VMIN]     =  vmin;     /*  get min of vmin chars */
    ttystate.c_cc[VTIME]    =  vtime;    /*  set timeout to vtime  */
    tcsetattr( 0 , TCSANOW, &ttystate);  /*  install settings      */
}

void set_non_blocking_mode(int fd)
/* sets O_NONBLOCK flag in descriptor for terminal */
{
    int   termflags;

    termflags = fcntl(fd, F_GETFL);       /*  read curr. settings     */
    termflags |= O_NONBLOCK;             /*  flip on nodelay bit     */
    fcntl(fd, F_SETFL, termflags);        /*  and install 'em     */
}


void tty_mode(int how)
/* if how == 0 saves the termios state  and fd flags for later restoring    */
/* if how == 1 restores the termios state and fd flags from the saved state */
/* CANNOT CALL with how == 1 before first calling with how == 0 */
{
    static struct termios original_mode;
    static int            original_flags = -1;

    if ( 0 == how ){
        tcgetattr(0, &original_mode);
        original_flags = fcntl(0, F_GETFL);
    }
    else {
        if ( -1 == original_flags ) {
            fprintf(stderr, "tty_mode(1) called without saving first.\n");
            exit(1);
        }
        tcsetattr(0, TCSANOW, &original_mode);
        fcntl( 0, F_SETFL, original_flags);
    }
}

void ctrl_c_handler(int signum)
/* purpose: called if SIGINT is detected */
/* action: reset tty and scram */
{
        tty_mode(1);
        printf("Control-C caught\n");
        exit(2);
}

