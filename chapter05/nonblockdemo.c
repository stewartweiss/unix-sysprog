/******************************************************************************
  Title          : nonblockdemo.c
  Author         : Stewart Weiss
  Created on     : April 11, 2013
  Description    : Displays a pattern on screen and also reads user input
  Purpose        : Demonstrates use of the O_NONBLOCK flag for nonblocking input
  Usage          : nonblockdemo
  Build with     : gcc -o nonblockdemo nonblockdemo.c
  Modifications  : 

  Notes          :
  This just draws a sequence of dots on the screen while displaying a prompt
  asking the user to enter a command. The user can type:
  q to quit
  p to pause, or
  r to resume.
  The whole point is just to show how non-blocking mode on the input file
  descriptor can be used while the program is busy doing something else.

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
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/stat.h>
#ifndef TIOCGWINSZ
#include <sys/ioctl.h>
#endif


#define RETRIEVE      1                 /* action for set_tty */
#define RESTORE       2                 /* action for set_tty */


/*****************************************************************************/

void  get_window_size( int fd, int *rows, int *cols )
{
    struct winsize size;
 
    if (ioctl(fd, TIOCGWINSZ,  &size) < 0) {
        perror("TIOCGWINSZ error");
        return;
    }
    *rows = size.ws_row;
    *cols = size.ws_col;
}

/*****************************************************************************/
void modify_termios(int fd, int echo, int canon )
{
    struct termios cur_tty;
    tcgetattr(fd, &cur_tty);

    if ( canon )
        cur_tty.c_lflag     |= ICANON;	
    else 
        cur_tty.c_lflag     &= ~ICANON;	
    if ( echo )
        cur_tty.c_lflag     |= ECHO;	
    else 
        cur_tty.c_lflag     &= ~ECHO;	
    cur_tty.c_lflag     &= ~ISIG;
    cur_tty.c_cc[VMIN]   =  1;		
    cur_tty.c_cc[VTIME]  =  0;
    tcsetattr(fd, TCSADRAIN, &cur_tty); 
}

/*****************************************************************************/
void save_restore_tty(int fd, int action)
{
    static struct termios original_state;
    static int            retrieved = 0;

    if ( RETRIEVE == action ){
        retrieved = 1;
        tcgetattr(fd, &original_state);
    }
    else if (retrieved &&  RESTORE == action ) {
        tcsetattr(fd, TCSADRAIN, &original_state); 
    }
    else
        fprintf(stderr, "Illegal action to save_restore_tty().\n");
}

/*****************************************************************************/
void set_non_block(int fd ) 
{ 
    int flagset; 

    flagset   = fcntl(fd, F_GETFL); 
    flagset   |= O_NONBLOCK;
    fcntl(fd, F_SETFL, flagset);	 
}



/******************************************************************************/
/*                           Main Program                                     */
/******************************************************************************/

int main (int argc, char *argv[])
{
    char     ch;         // stores user's char
    char   period = '.'; 
    size_t bytecount;  
    int    count = 0;   
    int    done  = 0;   /* to control when to stop loop */
    int    pause = 0;   /* to control pausing of output */
    char   PARK[20];    /* ANSI escape sequence for parking cursor */
    int    numrows;     /* number of rows in window */
    int    numcols;     /* number of columns in window */
    const  char CURSOR_HOME[]  = "\033[1;1H";
    const  char CLEAR_SCREEN[] = "\033[2J";
    const  char SAVE_CURSOR[]  = "\033[s";
    const  char REST_CURSOR[]  = "\033[u";
    const  char MENU[] = "Type q to quit or p to pause or r to resume.";
    char   dots[20];


    /* Check whether input or output has been redirected */
    if ( !isatty(0) || !isatty(1) ) {
        fprintf(stderr,"Output has been redirected!\n");
        exit(EXIT_FAILURE); 
    }

    /* Save the original tty state */
    save_restore_tty(STDIN_FILENO, RETRIEVE);  

    /* Modify the terminal - turn off echo, keybd sigs, and canonical mode */
    modify_termios( STDIN_FILENO, 0, 0);

    /* Turn off blocking mode */
    set_non_block( STDIN_FILENO );

    /* Get the window's size */
    get_window_size(STDIN_FILENO, &numrows, &numcols);

    /* Create string to park cursor */
    sprintf(PARK, "\033[%d;1H", numrows+1);

    /* Clear the screen and put cursor in upper left corner */
    write(STDOUT_FILENO,CLEAR_SCREEN, strlen(CLEAR_SCREEN));
    write(STDOUT_FILENO,CURSOR_HOME, strlen(CURSOR_HOME));
        
    /* Start drawing. Stop when the screen is full */
    while ( !done  ) {
        if ( ! pause ) {
            count++;
            /* Is screen full except for bottom row? */
            if ( count > (numcols * (numrows-1)) ) {
                pause = 1;
                count--;
            }
            else
                write(STDOUT_FILENO, &period, 1);
        }
        usleep(10000);  /* delay a bit */
        sprintf(dots, "  dots:   %d ", count);

        /* Save the cursor, park it, write the menu prompt */
        write(STDOUT_FILENO,SAVE_CURSOR, strlen(SAVE_CURSOR));   
        write(STDOUT_FILENO, PARK, strlen(PARK));
        write(STDOUT_FILENO, MENU, strlen(MENU) );
        write(STDOUT_FILENO, dots, strlen(dots));
        /* Do the read. If nothing was typed, do nothing */
        if ( (bytecount = read(STDIN_FILENO, &ch, 1) ) > 0 ) {
            if ( ch == 'q' )
                done = 1;
            else if ( ch == 'p' )
                pause = 1;
            else if ( ch == 'r' )
                pause = 0;
        }
        /* Restore the cursor so the next dot follows the previous */
        write(STDOUT_FILENO,REST_CURSOR, strlen(REST_CURSOR));           
    }  
    /* Cleanup -- flush queue, clear the screen, and restore terminal */
    tcflush(STDIN_FILENO,TCIFLUSH);   
    write(1,CLEAR_SCREEN, strlen(CLEAR_SCREEN));
    write(1,CURSOR_HOME, strlen(CURSOR_HOME));
    save_restore_tty(STDIN_FILENO, RESTORE);			
    return 0;
}





