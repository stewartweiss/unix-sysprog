/******************************************************************************
  Title          : utils_utmp.h
  Author         : Stewart Weiss, based on Molay
  Created on     : February  1, 2010
  Description    : Reads records into a hidden buffer and delivers them one at
                   a time.
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

#ifndef __UTMPLIB_H__
#define __UTMPLIB_H__

#include <utmp.h>

typedef struct utmp utmp_record;
#define NULL_UTMP_RECORD_PTR  ((utmp_record *) NULL)


/*****************************************************************************
 open_utmp( filename )  opens the given utmp file for buffered reading
 returns: a valid file descriptor on success
          -1 on error
 no error handling other than that
*****************************************************************************/
int open_utmp( char * );


/*****************************************************************************
 next_utmp( )
 returns: a pointer to the next utmp record from the opened file and advances
         to the next  record
         NULL if no more records are in the file
 *****************************************************************************/
utmp_record *next_utmp();

/*****************************************************************************
 closet_utmp( )   closes the utmp file and frees the file descriptor
 *****************************************************************************/
void close_utmp();





#endif /* __UTMPLIB_H__ */


