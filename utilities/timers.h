/* 
 * timers.h    : Header file for interval timer functions
 * Author      : Stewart Weiss
 * Created     : March 21, 2006
 * Purpose     : Just a utility to demonstrate call to setitimer
 */

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


#ifndef _TIMERS_H
#define _TIMERS_H

#include        <stdio.h>
#include        <sys/time.h>
#include        <signal.h>

// Initialize an interval timer of type which with initial delay
// of initial msecs and a repeat interval of repeata msecs.
// Return 0 on success, -1 on failure
int set_timer( int which, long initial, long repeat );


#endif
