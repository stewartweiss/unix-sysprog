/******************************************************************************
  Title          : showpid.c
  Author         : Stewart Weiss
  Created on     : August 10, 2011
  Description    : Displays its own process-id, nothing more
  Purpose        : To illustrate the concept of a process-id
  Usage          : showpid
                   Best to run it several times to see that the pid changes
                   with each new instance
  Build with     : gcc -o showpid showpid.c
  Modifications  :

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

#include <unistd.h>
#include <stdio.h>

int  main(void)
{
    printf("I am the process with process-id %d\n", getpid());
    return(0);
}

