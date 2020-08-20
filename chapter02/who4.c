/******************************************************************************
  Title          : who4.c
  Author         : Stewart Weiss
  Created on     : February  1, 2010
  Description    : Display records from wtmp or utmp file
  Purpose        : Demonstrates how to process utmp structures
                   using the GNU getutent_r() function.
  Usage          : who4 [wtmp]
                   if wtmp argument supplied, it shows the contents of
                   wtmp file, otherwise utmp file
  Remarks        :
                  - displays logins if utmp, all user_process records if wtmp
                  - suppresses empty records
                  - formats time nicely
  Build with     : gcc -o who4 who4.c -DSHOWHOST -I../include -L../lib -lutils

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
#include <paths.h>
#include <string.h>

// In order to use the getutent_r() function, declared in <utmp.h>, the
// _GNU_SOURCE feature test macro must be defined, since it is only available
// as a GNU extension to UNIX. It must be defined before including the utmp.h
// header file.

#define _GNU_SOURCE
#include <utmp.h>
#include <fcntl.h>
#include "utils.h"   // needed for the show_time() function



/*****************************************************************************
  show info( struct utmp* )
  displays contents of the utmp struct in human readable form
 *****************************************************************************/
void show_info(struct utmp *);


/*****************************************************************************
                               Main Program
*****************************************************************************/
int main(int argc, char* argv[])
{
    struct utmp  utbuf,
                 *utbufp;

    if ( (argc > 1) && (strcmp(argv[1],"wtmp") == 0) )
        utmpname(_PATH_WTMP);
    else
        utmpname(_PATH_UTMP);

    setutent();
    while( getutent_r(&utbuf, &utbufp) == 0 )
        show_info( &utbuf );
    endutent();
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



