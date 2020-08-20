/******************************************************************************
  Title          : utils_utmp.c
  Author         : Stewart Weiss, based on [Molay]
  Created on     : February  1, 2010
  Description    :
  Purpose        :
  Build with     :

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
#include  <stdio.h>
#include  <unistd.h>
#include  <fcntl.h>
#include  <sys/types.h>
#include  <utmp.h>
#include "utils.h"
#include "utmp_utils.h"

#define NUM_RECORDS           20
#define SIZE_OF_UTMP_RECORD   (sizeof(utmp_record))
#define BUFSIZE               ( NUM_RECORDS * SIZE_OF_UTMP_RECORD )

static char  utmpbuf[BUFSIZE];          // buffer of records
static int   number_of_recs_in_buffer;  // records stored into the buffer
static int   current_record;            // next rec to read
static int   fd_utmp = -1;              // file descriptor for utmp file

int fill_utmp();

/*****************************************************************************
  open_utmp( filename )  opens the given utmp file for buffered reading
  returns: a valid file descriptor on success
          -1 on error
  no error handling other than that
*****************************************************************************/
int open_utmp( char * file_utmp )
{
    fd_utmp = open( file_utmp, O_RDONLY );  // will return -1 on error
    current_record   = 0;
    number_of_recs_in_buffer = 0;
    return fd_utmp;             // either a valid file descriptor or -1
}

/*****************************************************************************
  next_utmp( )
  returns: a pointer to the next utmp record from the opened file and advances
         to the next  record
         NULL if no more records are in the file
 *****************************************************************************/
utmp_record * next_utmp()
{
    utmp_record * recordptr;
    int           byte_position;

    if ( -1 == fd_utmp )
        // file was not opened correctly
        return NULL_UTMP_RECORD_PTR;

    if ( current_record == number_of_recs_in_buffer )
        // there are no unread records in the buffer
        // need to fill the buffer
        if ( fill_utmp() == 0 )
            // no utmp records left in the file
            return NULL_UTMP_RECORD_PTR;

    // There is at least one record in the buffer, so we can read it
    byte_position = current_record * SIZE_OF_UTMP_RECORD;
    recordptr = ( utmp_record *) &utmpbuf[byte_position];

    // advance current_record pointer and return record pointer
    current_record++;
    return recordptr;
}


/*****************************************************************************
  close_utmp( )   closes the utmp file
 *****************************************************************************/
void close_utmp()
{
    // if the file descriptor is a valid one, close the connection
    if ( fd_utmp != -1 )
        close( fd_utmp );
}

/*****************************************************************************
  reload_utmp( )
  tries to read NUM_RECORDS records from the utmp file into the buffer.
  if successful, it returns number of records actually read
  and sets current_record to the first record in the buffer
 *****************************************************************************/
int fill_utmp()
{
    int   bytes_read;

    // read NUM_RECORDS records from the utmp file into buffer
    // bytes_read is the actual number of bytes read
    bytes_read = read( fd_utmp , utmpbuf, BUFSIZE );
    if ( bytes_read < 0 ) {
        die("Failed to read from utmp file","");
    }

    // If we reach here, the read was successful
    // Convert the bytecount into a number of records
    number_of_recs_in_buffer = bytes_read/SIZE_OF_UTMP_RECORD;

    // reset current_record to start at the buffer start
    current_record  = 0;
    return number_of_recs_in_buffer;
}

