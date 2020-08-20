/******************************************************************************
  Title          : who2.c
  Author         : Stewart Weiss
  Created on     : January 30, 2010
  Description    : Improves on who1.c by suppressing empty records and
                   formatting time.
  Purpose        :
  Build with     : gcc -o who2 who2.c -DSHOWHOST -I../include -L../lib -lutils

  Notes          : This version, like who1.c, accesses the members of the utmp
                   struct directly. The problem with this solution is that the
                   utmp struct has changed over time and the sizes and types of
                   its members vary across UNIX systems. It may fail to compile
                   on some systems because those systems have a different
                   version of the utmp struct.
                   Nonetheless, it is an exercise in accessing the contents of
                   a binary file and extracting the data that the program needs.
                   It is not a "real" solution.

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
#include "utils.h"   // in ../utilities (needed for the die function)


/*****************************************************************************
  show info( struct utmp* )
  displays contents of the utmp struct in human readable form
 *****************************************************************************/
void show_info(struct utmp *);



/*****************************************************************************
                               Main Program
*****************************************************************************/
int main(int argc, char *argv[])
{
    struct utmp     utbuf;          /* read info into here */
    int             utmpfd;         /* read from this descriptor */

    /* open the utmp file, check for errors */
    utmpfd = open(UTMP_FILE, O_RDONLY);
    if ( -1 == utmpfd ) {
        die(UTMP_FILE, argv[0]);
    }

    while( read(utmpfd, &utbuf, sizeof(utbuf)) == sizeof(utbuf) )
        show_info( &utbuf );
    close(utmpfd);
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
        printf(" (%s)", utbufp->ut_host);   /* the host     */
#endif
    printf("\n");                           /* newline      */
}



