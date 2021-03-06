/******************************************************************************
  Title          : more_utils_v2.c
  Author         : Stewart Weiss, based on a program by Bruce Molay
  Created on     : February  3, 2010
  Description    : Second attempt at more program
  Purpose        : Provides the two utility functions needed in main()
  Build with     : gcc -c more_utils_v2.c

  Notes          :
  This version extends the first version as follows:
  It allows more to be used in a pipeline by reading the user input from the
  user terminal instead of the stdin stream.

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

#define    SCREEN_ROWS       23
#define    LINELEN           512


/** get_user_input(FILE *fp )
 *  Displays more's status and prompt and waits for user response,
 *  Requires that user press return key to receive input
 *  Returns one of SPACEBAR, RETURN, or QUIT on valid keypresses
 *  and INVALID for invalid keypresses.
 *  Reads from fp instead of from standard input.
 */
int get_user_input( FILE *fp )
{
    int    c;

    printf("\033[7m more? \033[m");   /* reverse on a vt100    */
    // Now we use getc instead of getchar. It is the same except
    // that it requires a FILE* argument
    while( (c = getc( fp )) != EOF )   /* wait for response     */
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
    int    num_of_lines = SCREEN_ROWS;  // number of lines left on screen
    int    getmore      = 1;            // boolean to signal when to stop
    int    reply;                       // input from user
    FILE    *fp_tty;

    fp_tty = fopen( "/dev/tty", "r" );  // NEW: FILE stream argument
    if ( NULL == fp_tty  )               // if open fails
        exit(1);                        // exit


    while ( getmore && fgets( line, LINELEN, fp ) ){
        // fgets() returns pointer to string read
        if ( 0 == num_of_lines ) {
            // reached screen capacity so display prompt
            reply = get_user_input( fp_tty );  // note call here
            switch ( reply ) {
                case SPACEBAR:
                    // allow full screen
                    num_of_lines = SCREEN_ROWS;
                    break;
                case RETURN:
                    // allow one more line
                    num_of_lines++;
                    break;
                case QUIT:
                    getmore = 0;
                    break;
                default:    // in case of invalid input
                    break;
            }
        }
        if ( fputs( line, stdout )  == EOF )
            exit(1);
        num_of_lines--;
    }
}
