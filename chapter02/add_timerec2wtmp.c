/******************************************************************************
  Title          : add_timerec_wtmp.c
  Author         : Stewart Weiss,
  Created on     : March 3, 2011
  Description    : Adds either an OLD_TIME or NEW_TIME record to wtmp file
  Purpose        :
  Usage          : add_timerec_wtmp <filename> <datestring> new|old
                   where
                       <filename> is the path to a wtmp file
                       <datestring> is enclosed in single quotes and
                       is of the form 'Thu Mar  3 22:38:13 2011'
                       which is what date produces minus the timezone
                       new specifies a NEW_TIME record,
                       old, an OLD_TIME record.

  Build with     : gcc -o add_timerec_wtmp add_timerec_wtmp.c -lutils

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

#define _GNU_SOURCE 500
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <utmp.h>
#include <fcntl.h>
#include <string.h>

#include "utils.h"

void fill_utmp(struct utmp *, time_t, int  );

char usage[] = "usage:"
                "  add_timerec_wtmp <filename> <datestring> new|old\n"
                "  where\n"
                "  <filename> is the path to a wtmp file\n"
                "  <datestring> is enclosed in single quotes and\n"
                "    is of the form 'Thu Mar  3 22:38:13 2011'\n"
                "    which is what date produces minus the timezone\n"
                "    new specifies a NEW_TIME record, and \n"
                "    old, an OLD_TIME record.\n";



/*****************************************************************************
                               Main Program
*****************************************************************************/
int main(int argc, char* argv[])
{
    struct utmp  utbuf;
    struct tm    *tmp;   // stores time converted by getdate()
    time_t caltime;

    if ( argc < 4 ) {
        printf("%s", usage);
        return 0;
    }
    tmp = getdate(argv[2]);    // convert a time string to a tm struct
    if (tmp == NULL) {
        printf("getdate_err = %d\n", getdate_err);
        exit(1);
    }

    caltime = mktime(tmp);    // convert a tm struct to a time_t

    if ( strcmp(argv[3],"new") == 0 )
        fill_utmp(&utbuf, caltime, NEW_TIME);
    else
        fill_utmp(&utbuf, caltime, OLD_TIME);

    updwtmp(argv[1], &utbuf);
    return 0;
}

void fill_utmp( struct utmp *utbufp, time_t time_to_set, int which)
{

    utbufp->ut_type = which;
    utbufp->ut_line[0] = '\0';
    utbufp->ut_user[0] = '\0';
    utbufp->ut_host[0] = '\0';
    utbufp->ut_tv.tv_sec = time_to_set;
    utbufp->ut_tv.tv_usec = 0;
    utbufp->ut_exit.e_exit = 0;
    utbufp->ut_exit.e_termination = 0;
}





