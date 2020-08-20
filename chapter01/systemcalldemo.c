/******************************************************************************
  Title          : systemcalldemo.c
  Author         : Stewart Weiss
  Created on     : March 16, 2013
  Description    : Uses syscall() to make a system call
  Purpose        : To demonstrate how to use syscall
  Usage          : systemcalldemo2
  Build with     : gcc -o systemcalldemo systemcalldemo.c
  Modifications  :

  Notes
  This is an example of a system call for which there is no declaration, i.e.,
  wrapper function, defined in the GNU C library (glibc). The only way to
  call it is via the syscall() interface.

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

#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    printf("Thread id %ld\n", syscall(SYS_gettid));
    return 0;
}

