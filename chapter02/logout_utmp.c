/******************************************************************************
  Title          : logout_utmp.c
  Author         : Stewart Weiss
  Created on     : March  4, 2011
  Description    : Finds login record and changes it to logout record in utmp
  Purpose        : To demonstrate opening a file in read/write mode and updating
                   a log file
  Usage          : logout_utmp <utmp-file> <terminal-line>
                   where
                        <utmp-file> is a utmp file that is  writable by the user
                   and
                        <terminal-line> is the line e.g., pts/4, on which the
                        logout being simulated took place. If the lines in the
                        utmp file are of the form "/dev/pts/4" then the user
                        mustenter them in that form as well.

  Notes            Create a copy of the utmp file into the working directory
                   and make it writable by the user if it is not. Use a command
                   such as last -f <utmp-copy> to view login records, or use
                   the show_utmp command that I wrote. Pick a login that is
                   still active and use the line from that login as the line
                   to close.

                   DO NOT NAME THIS PROGRAM logout -- IT IS SAFER TO NAME IT
                   DIFFERENTLY.

  Build with     : gcc -o logout_utmp logout_utmp.c

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
#include <fcntl.h>
#include <utmp.h>


/*****************************************************************************
                               Main Program
*****************************************************************************/
int main(int argc, char* argv[])
{
    struct utmp  utbuf;                   // stores a single utmp record
    int          fd;                      // file descriptor for utmp file
    int          utsize  = sizeof(utbuf); // size of a utmp record in bytes
    struct timeval tv;

    // check usage
    if ( argc < 3 ){
        fprintf( stderr, "usage: %s <utmp-file> <line>\n", argv[0]);
        exit(1);
    }

    // open utmp file
    if ( (fd = open(argv[1], O_RDWR)) == -1 ) {
        fprintf(stderr, "Cannot open %s\n", argv[1]);
        exit(1);
    }

    // If the line is longer than a ut_line permits do not continue
    if ( strlen(argv[2]) >= UT_LINESIZE ) {
        fprintf(stderr, "Improper argument:%s\n", argv[1]);
        exit(1);
    }

    while ( read(fd, &utbuf, utsize ) == utsize )
        // Check if line matches ut_line in current record and it has a non-null
        // user name
        if ( ( strncmp(utbuf.ut_line, argv[2], sizeof(utbuf.ut_line) ) == 0) &&
              ( utbuf.ut_user[0] ) ) {
            // If a match, change type, clear user and host members and set time
            utbuf.ut_type    = DEAD_PROCESS;
            utbuf.ut_user[0] = '\0';
            utbuf.ut_host[0] = '\0';



            if ( gettimeofday(&tv, NULL) != 0 ) {
                fprintf( stderr, "error getting time of day\n");
                exit(1);
            }
            utbuf.ut_tv.tv_sec = tv.tv_sec;
            utbuf.ut_tv.tv_usec = tv.tv_usec;

                // Now replace record in file with the changed struct
            if ( lseek(fd, -utsize, SEEK_CUR) == -1 ) {
                fprintf( stderr, "error seeking in utmp file\n");
                exit(1);
            }
            if ( write(fd, &utbuf, utsize) != utsize ) {
                fprintf( stderr, "failed write in utmp file\n");
                exit(1);
            }
            break;
        } // end if

    close(fd);
    return 0;
}

