/******************************************************************************
  Title          : term_demo1.c
  Author         : Stewart Weiss
  Created on     : March 22, 2008
  Description    : Allows terminal settings to be controlled to see effects
  Purpose        : Allows user to control O_NONBLOCK, ICANON, VMIN, VTIME
                   independently in order to see the effect on interactive
                   reads from the terminal. Does not handle signals.
  Usage          : 
                   term_demo1 [OPTIONS]
                   OPTIONS 
                    [-hv] [-b 0|1] [-c 0|1] [ -m MIN ] [-s SECS] [ -t TIME ]
                    -h  : help
                    -v  : verbose
                    -b  : turn on blocking mode with 1, off with 0
                    -c  : turn on canonical mode with 1, off with 0
                    -m  : set VMIN value 
                    -s  : sleeptime in seconds
                    -t  : set VTIME value, tenths of a second
  Build with     : gcc -Wall -g -o term_demo1 term_demo1.c
  Modifications  : March 23, 2011


                   Restructured saving and restoring and changing terminal
                   settings
                   April 2, 2012
                   Fixed get_response, which was producing double prompts in
                   canonical mode.
 
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
#include <ctype.h>
#include <stdlib.h>
#include <termios.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

#define	PROMPT             "Do you want to continue?"
#define	MAX_TRIES          3	               /* max tries      */
#define SLEEPTIME_DEFAULT  2                 /* time per try   */
#define	BEEP               putchar('\a')     /* alert user     */
#define RETRIEVE           1                 /* action for set_tty */
#define RESTORE            2                 /* action for set_tty */
#define FALSE              0
#define TRUE               1

/*****************************************************************************/
/*  The following struct is for storing termios options set by the           */
/*  command line.                                                            */
/*****************************************************************************/

typedef struct tty_opts_tag {
    int  min;                 /* value to assign to MIN */
    int  time;                /* value to assign to TIME */
    int  echo;                /* value to assign to echo  [0|1] */
    int  canon;               /* value to assign to canon [0|1] */
} tty_opts;

/*****************************************************************************/
/*  The following struct is for storing parameter options to pass to the     */
/*  get_response function.                                                   */
/*****************************************************************************/

typedef struct ui_param_tag {
    int  sleeptime;
    int  isblocking;
    int  maxtries;
    char prompt[256];
} ui_params;

/* Gloab variable for verbose mode */

int verbose = 0;

/*****************************************************************************/
/*  The following is a string for displaying help.                           */
/*****************************************************************************/


static const char *help = 
" [-hv] [-b 0|1] [-c 0|1] [ -m MIN ] [ -t TIME ]\n"
"       -h      : help\n"
"       -v      : verbose mode\n"
"       -b  1/0 : turn on/off blocking mode     (default = on)\n"
"       -c  1/0 : turn on/off canonical mode    (default = on)\n"
"       -e  1/0 : turn on/off echo              (default = on)\n"
"       -m value: set VMIN to value (chars)     (default = 1)\n"
"       -s value: sleeptime between requests for user input (secs)\n"
"       -t value: set VTIME to value (tenths second \n"
"When blocking is on, the value of s is ignored. When canonical mode is on,\n"
"the value of s is ignored unless blocking is turned off.\n"
"\n"
"SUGGESTIONS\n"
"For each different mode that you put this program into, try typing legal inputs\n"
"'y' and 'n', illegal inputs such a other letters, and then not typing anything.\n"
"Depending on the mode, typing nothing will either block the program indefinitely\n"
"or cause a timeout. Timeouts are counted and the program tells you how many are\n"
"left. \n"
"\n"
"The main program will only terminate when you type an 'n' in response to the\n"
"prompt.\n"
"\n"
"Try turning echo on and off during the following mode changes: -e0\n"
"\n"
"Run this program first in canonical mode with normal blocking input: -b1 -c1\n"
"You will have to use the Enter key to send the input.\n"
"\n"
"Then see the change in behavior by turning off blocking mode: -b0 -c1\n"
"You will still need to use the Enter key to send the input.\n"
"\n"
"In this mode, the sleeptime parameter will introduce a time delay between the\n"
"prompt for input and the execution of the read() system call, so that you have \n"
"time to prepare to enter input. If sleeptime is too small, you may not have\n"
"enough time to decide what to type. Adjust the value of the sleeptime parameter \n"
"until the delay suits you.\n"
"\n"
"Next, turn blocking input back on and turn off canonical mode: -b1 -c0\n"
"In this mode experiment with the values of the MIN and TIME variables to see how\n"
"the four combinations of (min,time) work.\n"
"\n"
"Finally, turn off both blocking and canonical mode and make sure sleeptime is a \n"
"comfortable value for you.\n"
"\n"
"\n"
;

/*****************************************************************************/
/*                     Utility Function Prototypes                           */
/*****************************************************************************/

/*
 * Parses the command line and fills the file_flags, tty_opts struct, and
 * sleeptime parameter with the values it obtained.  It is given the
 * addresses of the argc and argv parameters so that it can update them
 * if needed (which it presently does not do.)
 */
void get_options( int *argc, char ***argv, int* file_flags, tty_opts * ts,
                            int *sleeptime  );


/* 
 * 
 * If action == RETRIEVE this saves the termios state for later restoring
 * If action == RESTORE  this restores the termios state from the saved state
 * RESTORING before RETRIEVING is an error
 */
void save_restore_tty(int fd, int action, struct termios *copy);


/*
 * Applies the values of the options in the tty_opts struct ts to the termios 
 * struct cur_tty passed to it. 
 * This function can modify only the canonical mode, echo, VMIN, and VTIME
 * attributes.
 */
void modify_termios( struct termios *cur_tty, tty_opts ts );


/*
 * Applies the terminal settings in the termios struct cur_tty to the 
 * terminal associated to the file descriptor fd.
 */
void apply_termios_settings( int fd, struct termios cur_tty );

/*
 * Sets the O_NONBLOCK bit on file descriptor fd according to whether it is
 * on or off in file_flags.
 */
void set_non_blocking_mode(int fd, int file_flags);

/*
 * get_response simulates an interactive loop in which a user is expected
 * to answer y or n to a yes/no question. It reads from the file stream *fp
 * and is configured by the parameter values in ui_params up. These control
 * 1. the number of seconds to sleep between prompt and attempt to read input
 * 2. whether the file connection is in non-blocking mode or not;
 * 3. the number of timeouts to wait until it gives up, and
 * 4. the prompt string to display.
 * It displays the number of timeouts remaining  and the elapsed time as it waits.
 */
int  get_response( FILE* fp, ui_params up );


/*
 *  Called to display options in effect in verbose mode 
 */
void explain( FILE*, tty_opts, int );



/*****************************************************************************/
/*                                    Main                                   */
/*****************************************************************************/

int main(int argc, char* argv[])
{
    int             response;
    tty_opts        ttyopts = {
                        1,      /* min = 1              by default */
                        0,      /* time = 0             by default */
                        1,      /* echo is on           by default */
                        1       /* canonical mode is on by default */
                    };
    ui_params       ui_parameters = 
                     {SLEEPTIME_DEFAULT, TRUE, MAX_TRIES, PROMPT };
    int             fflags = 0;
    struct termios  ttyinfo;
    FILE*           fp;
    
    get_options(&argc, &argv, &fflags, &ttyopts, &(ui_parameters.sleeptime));

    if ( !isatty(STDIN_FILENO) || !isatty(STDOUT_FILENO) )
        exit(1);

    fp = stdin;
    if ( verbose )
        explain( fp, ttyopts, fflags);

    /* save the origingal tty state and copy it into ttyinfo */
    save_restore_tty(fileno(fp), RETRIEVE, &ttyinfo);  

    /* Modify ttyinfo with options specified on command line        */
    modify_termios( &ttyinfo, ttyopts);

    /* Apply the termios struct ttyinfo to the termios of fp */
    apply_termios_settings( fileno(fp), ttyinfo);

    /* If fflags != 0 then the O_NONBLOCK flag is set on fp */
    if ( fflags ) {
        ui_parameters.isblocking = FALSE;
        set_non_blocking_mode(fileno(fp), fflags);	
    }

    while ( TRUE ) {
        response = get_response(fp, ui_parameters );
        if ( response ) {
            printf("\nMain: Response from user = %c\n", response );
            if ( 'n' == response )
                 break;
            }
        else
            printf("\nMain: No response from user\n" );
    }


    save_restore_tty(fileno(fp), RESTORE, NULL);			
    tcflush(0,TCIFLUSH);   
    return response;
}

/*****************************************************************************/

void usage( char * prog)
{
    printf("Usage: %s", prog);
    printf("%s\n", help);

}

/*****************************************************************************/

void get_options( int       *argc, 
                  char      ***argv, 
                  int*      file_flags, 
                  tty_opts  *ts, 
                  int       *sleeptime )
{
    int ch;
    char options[] = ":hvb:c:e:m:s:t:";
    /* b1 blocking on, b0 blocking off
       c1 canonical mode on, c0 canconical mode off
       e1 echo on, e0 echo off
       m  x value of MIN 
       t  x value of TIME
    */

    /* turn off error messages by getopt() */
    opterr = 0;  

    /* process the command line options */
    while  (TRUE) {
        ch = getopt(*argc, *argv, options);
        if ( -1 == ch )
            break;
        switch ( ch ) {
        case 'h' :
            usage((*argv)[0]);
            exit(0);
        case 'b':
            if ( 0 == strtol(optarg,'\0',0) )
                *file_flags |= O_NONBLOCK;  
             break;
        case 'v':
            verbose = 1;
            break;
        case 'c':
            ts->canon   = strtol(optarg,'\0',0);  
            break;
        case 'e':
            ts->echo    = strtol(optarg,'\0',0);            
            break;
        case 'm':
            ts->min     = strtol(optarg,'\0',0);
            break;
        case 's':
            *sleeptime  = strtol(optarg,'\0',0);
            break;
        case 't':
            ts->time    = strtol(optarg,'\0',0);
            break;
        case '?' :
        default:
           fprintf (stderr, "?? bad option 0%o ??\n", ch);
           break;
        }
    }


}

/*****************************************************************************/
int get_response( FILE* fp, ui_params  uip )
{
    int input, n;
    unsigned char c;
    time_t time0, time_now;
   
    time(&time0);
    while ( TRUE ){
        printf( "    %s (y/n)?", uip.prompt);			

        /* Flush the output buffer, since printf did not newline-terminate*/
        fflush(stdout);	

        /* If we're in non-blocking mode, sleep to delay start of read() call
           otherwise user will have no time to respond */
        if ( !uip.isblocking )
             sleep(uip.sleeptime);	

        /* Do the read of a single char */	
        if ( (n = read(fileno(fp), &c, 1)) > 0 ) {
            input = tolower(c); /* convert to lowercase */

            /* In canonical mode there might be a newline in the terminal's
               input queue. Call tcflush() to empty that queue just in case. */
            tcflush(fileno(fp), TCIFLUSH);

            if ( 'y' == input  || 'n' == input ) {
                return input;
            }
            else {
                printf("\n    Invalid input: %c\n", input);
                continue;
            }
        }
        /* We count iterations of the loop and when we reach maxtries 
           iterations, we exit. */
        BEEP;              /* beep if the sound is on */
        time(&time_now);   /* get the current time    */
        printf( "\n    Timeout waiting for input: %d seconds elapsed,"
                " %d timeouts left\n",
                 (int)(time_now- time0), uip.maxtries);
        if ( 0 == uip.maxtries-- ) {
            printf("\n    Time is up.\n");
            return 0;
        }
    }
}

/*****************************************************************************/

void modify_termios( struct termios *cur_tty, tty_opts ts )
{

    if ( ts.canon )
        cur_tty->c_lflag    |= ICANON;
    else
        cur_tty->c_lflag    &= ~ICANON;

    if ( ts.echo )
        cur_tty->c_lflag    |= ECHO;	
    else
        cur_tty->c_lflag    &= ~ECHO;	

    cur_tty->c_cc[VMIN]  	=  ts.min;		
    cur_tty->c_cc[VTIME]  	=  ts.time;
}

/*****************************************************************************/

void apply_termios_settings( int fd, struct termios cur_tty )
{
    if ( verbose )
        printf("Applying settings to terminal of file descriptor %d\n", fd);
    tcsetattr(fd, TCSANOW, &cur_tty); 
}

/*****************************************************************************/
void set_non_blocking_mode(int fd, int file_flags)
{
    int flagset;

    flagset   = fcntl(fd, F_GETFL);		
    flagset   |= O_NONBLOCK; 			                                               
    fcntl(fd, F_SETFL, flagset);	
    if ( verbose )
        printf("Non-blocking mode set on %s.\n", ttyname(fd));
}

/*****************************************************************************/

void save_restore_tty(int fd, int action, struct termios *copy)
{
    static struct termios original_state;
    static int            original_flags = -1;
    static int            retrieved = FALSE;

    if ( RETRIEVE == action ){
        retrieved = TRUE;
        tcgetattr(fd, &original_state);
        original_flags = fcntl(fd, F_GETFL);
        if ( copy != NULL )
            *copy = original_state;
    }
    else if (retrieved &&  RESTORE == action ) {
        tcsetattr(fd, TCSADRAIN, &original_state); 
        fcntl( fd, F_SETFL, original_flags);    
    }
    else
        fprintf(stderr, "Illegal action to save_restore_tty().\n");
}
/*****************************************************************************/

void explain( FILE* fp, tty_opts ttyopts, int fflags )
{
    printf("File descriptor for connection is %d\n", fileno(fp));
    if ( ttyopts.canon)
        printf("Canonical mode on\n");
    else 
        printf("Canonical mode off\n");
    if ( ttyopts.echo)
        printf("Echo on\n");
    else 
        printf("Echo off\n");

    if ( fflags )
        printf("Blocking mode off\n");
    else 
        printf("Blocking mode on\n");
    printf("VMIN  = %d\n",ttyopts.min);
    printf("VTIME = %d\n",ttyopts.time);
}

