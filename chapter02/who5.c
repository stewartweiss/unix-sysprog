/******************************************************************************
  Title          : who5.c
  Author         : Stewart Weiss, based on Molay
  Created on     : February  1, 2010
  Description    : Improves on who2.c by introducing buffered reads of utmp file
  Purpose        : To demonstrate how to do user controlled buffering
  Usage          : who5

  Build with     : gcc -o who5 who5.c utmp_utils.c -DSHOWHOST -I../include \
                   -L../lib -lutils

  Notes          : This program uses the functions in the file utmp_utils.c.
                   That file implements the buffering of the utmp file records.
                   This main program uses calls to open_utmp(), next_utmp(),
                   and close_utmp() defined there.

******************************************************************************
 * Copyright (C) 2020 - Stewart Weiss
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
#include <utmp.h>
#include <fcntl.h>
#include <time.h>
#include "utmp_utils.h"
#include "utils.h"   // in ../utilities (needed for the die function)



/*****************************************************************************
  show info( struct utmp* )
  displays contents of the utmp struct in human readable form
 *****************************************************************************/
void show_info(utmp_record *);


/*****************************************************************************
                               Main Program
*****************************************************************************/
int main(int argc, char* argv[])
{

    utmp_record	*utbufp;        // points to a utmp record

    if ( open_utmp( UTMP_FILE ) == -1 ){
    	perror(UTMP_FILE);
    	exit(1);
    }
    while ( ( utbufp = next_utmp() ) != NULL_UTMP_RECORD_PTR  )
    	show_info( utbufp );

    close_utmp( );
    return 0;
}

/*****************************************************************************
  show info()
  displays contents of the utmp struct in human readable form
  The sizes used in the printf below are not guaranteed to work on all systems.
  The ut_time member may be 32 or 64 bits.
  If the type of entry is not USER_PROCESS, it skips the record
 *****************************************************************************/
void show_info( struct utmp *utbufp )
{
    if ( utbufp->ut_type != USER_PROCESS )
            return;

    printf("%-8.8s", utbufp->ut_name);      /* the logname  */
    printf(" ");                            /* a space      */
    printf("%-12.12s", utbufp->ut_line);    /* the tty      */
    printf(" ");                            /* a space      */
    show_time( utbufp->ut_time );           /* display time */
#ifdef SHOWHOST
    if ( utbufp->ut_host[0] != '\0' )
        printf(" (%s)", utbufp->ut_host);   /* the host    */
#endif
    printf("\n");                           /* newline      */
}


