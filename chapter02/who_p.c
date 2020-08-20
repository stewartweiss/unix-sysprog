/******************************************************************************
  Title          : who_p.c -- a POSIX-compliant version of who3.c
  Author         : Stewart Weiss
  Created on     : February  13, 2011
  Description    : Display records from wtmp or utmp file
  Purpose        : Demonstrates how to process utmp structures
                   using the POSIX.1 setutxent(), getutxent(), and endutxent()
                   functions.
  Usage          : who_p [wtmp]
                   if wtmp argument supplied, it shows the contents of
                   wtmp file, otherwise it shows those of the utmp file
  Remarks        :
                  - displays logins if utmp, all user_process records if wtmp
                  - suppresses empty records
                  - formats time nicely
  Build with     : gcc -o who_p who_p.c -I../include -L../lib -lutils

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
#include <string.h>
#include <utmp.h>

/*
   Unlike the old utmp.h file, the utmpx.h file does not automatically include
   the <paths.h> header file, which is where _PATH_UTMP and _PATH_WTMP are
   defined. The only way to know this is to read the files; it is not documented
   anywhere sensible.  The <utmpx.h> header file WILL include <paths.h> if the
   feature test macro __USE_GNU is defined. Therefore, this program includes
   <paths.h> only if __USE_GNU is not defined. Of course we could include it
   anyway, but this is here to demonstrate how to conditionally include headers
   depending on the features of the particular system.
*/
#ifndef __USE_GNU
    #include <paths.h>
#endif
#include <utmpx.h>
#include <fcntl.h>
#include "utils.h"   // needed for the show_time() function



/*****************************************************************************
  show info( struct utmp* )
  displays contents of the utmp struct in human readable form
 *****************************************************************************/
void show_info(struct utmpx *);


/*****************************************************************************
                               Main Program
*****************************************************************************/
int main(int argc, char* argv[])
{
    struct utmpx  *utbufp;

    if ( (argc > 1) && (strcmp(argv[1],"wtmp") == 0) )
        utmpname(_PATH_WTMP);
    else
        utmpname(_PATH_UTMP);

    setutxent();

    while( (utbufp = getutxent()) != NULL )
        show_info( utbufp );

    endutxent();
    return 0;
}

/*****************************************************************************
  show info()
  displays contents of the utmp struct in human readable form
  The sizes used in the printf below are not guaranteed to work on all systems.
  The ut_time member may be 32 or 64 bits.
  If the type of entry is not USER_PROCESS, it skips the record

  The conditional macro that tests whether _NO_UT_TIME is defined is needed
  because the POSIX utmpx struct may or may not have a ut_time member. The
  ut_time member is not POSIX, but some systems supply a macro to define it.
  We could just use the ut_tv.tv_sec member of the struct, but this code
  intentionally shows you how to handle both situations.
 *****************************************************************************/
void show_info( struct utmpx *utbufp )
{
    if ( utbufp->ut_type != USER_PROCESS )
            return;

    printf("%-8.8s", utbufp->ut_user);      /* the logname  */
    printf(" ");                            /* a space      */
    printf("%-12.12s", utbufp->ut_line);    /* the tty      */
    printf(" ");                            /* a space      */
#ifndef _NO_UT_TIME
    #define ut_time ut_tv.tv_sec
#endif
    show_time( utbufp->ut_time );           /* display time */
#ifdef SHOWHOST
    if ( utbufp->ut_host[0] != '\0' )
        printf(" (%s)", utbufp->ut_host);   /* the host    */
#endif
    printf("\n");                           /* newline      */
}



