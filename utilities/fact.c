#include <limits.h>

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
long fact(int n)  /*  compute  n! */
{
    long k = 1;
    long old_k;
    while ( (n > 0) && ( k < LONG_MAX) ) {
        old_k = k;
        k *= n--;
    }
    if ( n > 0 )
        return -1;
    else
        return(k);
}

