/******************************************************************************
  Title          : more_utils_v3.c
  Author         : Stewart Weiss, based on Bruce Molay, Chapter 1
  Created on     : February  3, 2010
  Description    : Third attempt at more program
  Purpose        : Provides the two utility functions needed in main()
  Build with     : gcc -c more_utils_v3.c

  Notes          :
  This version extends Version 2 as follows:
  It determines the terminal size dynamically so that it can place the prompt
  in the correct position if the size changes, and so that it can always page
  the correct number of lines.
  It erases the previous prompt before displaying the new one.

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
#include  "more_utils.h"
#include  <sys/ioctl.h>

#define   LINELEN 512

void get_tty_size(FILE *tty_fp, int* numrows, int* numcols)
{
    struct winsize window_arg;

    if (-1 == ioctl(fileno(tty_fp), TIOCGWINSZ, &window_arg)) {
        exit(1);

    }
    *numrows =  window_arg.ws_row;
    *numcols =  window_arg.ws_col;
}

/** get_user_input(FILE *fp )
 *  Displays more's status and prompt and waits for user response,
 *  Requires that user press return key to receive input
 *  Returns one of SPACEBAR, RETURN, or QUIT on valid keypresses
 *  and INVALID for invalid keypresses.
 *  Reads from fp instead of from standard input.
 */
int get_user_input( FILE *fp )
{
    int   c;
    int   tty_rows;
    int   tty_cols;

    /*
     * Get the size of the terminal window dynamically, just in case it changed.
     * Then use it to put the cursor in the bottom row, leftmost column
     * and print the prompt in "standout mode" i.e. reverse video.
     */
    get_tty_size(fp, &tty_rows, &tty_cols);
    printf("\033[%d;1H", tty_rows);
    printf("\033[7m more? \033[m");


    /* Use fgetc() instead of getc().  It is the same except
     * that it is always a function call, not a macro, and it is in general
     * safer to use.
     */
    while ( (c = fgetc( fp )) != EOF )  {
        /* There is no need to use a loop here, since all possible paths
         * lead to a return statement. It remains since there is no downside
         * to using it.
         */
        switch ( c ) {
            case 'q' :                /* 'q' pressed */
                return QUIT;
            case ' ' :                /* ' ' pressed */
                return SPACEBAR;
            case '\n' :               /* Enter key pressed */
                return RETURN;
            default :                 /* invalid if anything else */
                return INVALID;
         }
    }
    return INVALID;
}

/** do_more_of ( FILE * fp )
 *  Given a FILE* argument fp, display up to a page of the
 *  file fp, and then display a prompt and wait for user input.
 *  If user inputs SPACEBAR, display next page.
 *  If user inputs RETURN, display one more line.
 *  If user inputs QUIT, terminate program.
 */
void do_more_of( FILE *fp )
{
    char   line[LINELEN];               // buffer to store line of input
    int    num_of_lines;                // number of lines left on screen
    int    reply;                       // input from user
    int    tty_rows;                    // number of rows in terminal
    int    tty_cols;                    // number of columns in terminal
    FILE   *fp_tty;                     // device file pointer

    fp_tty = fopen( "/dev/tty", "r" );  // NEW: FILE stream argument
    if ( NULL == fp_tty  )               // if open fails
        exit(1);                        // exit

    /* Get the size of the terminal window */
    get_tty_size(fp_tty, &tty_rows, &tty_cols);
    num_of_lines = tty_rows;

    while ( fgets( line, LINELEN, fp ) ){
        if ( 0 == num_of_lines ) {
            // reached screen capacity so display prompt
            reply = get_user_input( fp_tty );  // note call here
            switch ( reply ) {
                case SPACEBAR:
                    // allow full screen
                    num_of_lines = tty_rows;
                    printf("\033[1A\033[2K\033[1G");
                    break;
                case RETURN:
                    // allow one more line
                    printf("\033[1A\033[2K\033[1G");
                    num_of_lines++;
                    break;
                case QUIT:
                    printf("\033[1A\033[2K\033[1B\033[7D");
                    return;
                default:    // in case of invalid input
                    break;
            }
        }
        if ( fputs( line, stdout )  == EOF )
            exit(1);
        num_of_lines--;
    }
}
