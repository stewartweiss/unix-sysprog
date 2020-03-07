/******************************************************************************
  Title          : die.c
  Author         : Stewart Weiss
  Created on     : January 30, 2010
  Description    : prints error messages and then terminates calling process
  Purpose        : 
  Build with     : 
 
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

#include <stdio.h>
#include <stdlib.h>
#include "die.h"


void die(char *string1, char *string2)
{
        fprintf(stderr,"Error: %s ", string1);
        perror(string2);
        exit(1);
}
