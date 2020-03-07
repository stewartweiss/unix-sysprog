/******************************************************************************
  Title          : timers.c
  Author         : Stewart Weiss
  Created on     : March 21, 2006
  Description    : Set the value of the specified timer to the number of
                   milliseconds specified for intial and repeat values           
  Modified on    : April 5, 2008
                   Changed logic for computing fields to speed it up


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


#include        <stdio.h>
#include        <sys/time.h>
#include        <signal.h>

int set_timer( int which, long initial, long repeat )
{
    struct itimerval itimer;
    long secs;

    // initialize initial delay
    secs = initial / 1000 ; 
    itimer.it_value.tv_sec     = secs;    
    itimer.it_value.tv_usec    = (initial - secs*1000 ) * 1000 ;   

    // initialize repeat inveral
    secs = repeat / 1000 ; 
    itimer.it_interval.tv_sec  = secs;     
    itimer.it_interval.tv_usec = (repeat - secs*1000 ) * 1000 ;
    
    return setitimer(which, &itimer, NULL);
}

