/******************************************************************************
  Title          : more.c
  Author         : Stewart Weiss, based on Bruce Molay, Chapter 1
  Created on     : February  3, 2010
  Description    : To page a file, one screen at a time.
  Purpose        : Main program for the demo version of more
  Usage          : more [files]
                   Without file arguments it displays standard input
  Build with     : gcc -o more more.c

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

int main( int argc , char *argv[] )
{
    FILE    *fp;
    int     i = 0;

    if ( 1 == argc )
        do_more_of( stdin );
    else
        while ( ++i < argc ) {
            fp = fopen( argv[i] , "r" );
            if ( NULL != fp ) {
                do_more_of( fp ) ;
                fclose( fp );
            }
            else
                printf ( "Skipping %s\n", argv[i] );
        }
    return 0;
}
