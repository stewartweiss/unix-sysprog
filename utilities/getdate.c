#define _GNU_SOURCE 500
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

/******************************************************************************
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


int  main(int argc, char *argv[])
{
   struct tm *tmp;
   int j;
   time_t caltime;

   struct timeval tv;

   for (j = 1; j < argc; j++) {
       tmp = getdate(argv[j]);

       if (tmp == NULL) {
           printf("Call %d failed; getdate_err = %d\n",
                  j, getdate_err);
           continue;
       }

       printf("Call %d (\"%s\") succeeded:\n", j, argv[j]);
       printf("    tm_sec   = %d\n", tmp->tm_sec);
       printf("    tm_min   = %d\n", tmp->tm_min);
       printf("    tm_hour  = %d\n", tmp->tm_hour);
       printf("    tm_mday  = %d\n", tmp->tm_mday);
       printf("    tm_mon   = %d\n", tmp->tm_mon);
       printf("    tm_year  = %d\n", tmp->tm_year);
       printf("    tm_wday  = %d\n", tmp->tm_wday);
       printf("    tm_yday  = %d\n", tmp->tm_yday);
       printf("    tm_isdst = %d\n", tmp->tm_isdst);

       caltime = mktime(tmp);
       printf("Time since epoch in seconds = %d\n", (int) caltime);

       tv.tv_sec = caltime;
       tv.tv_usec = 0;
   }
}

