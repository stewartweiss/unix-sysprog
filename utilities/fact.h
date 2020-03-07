#ifndef __FACT_H
#define __FACT_H

/******************************************************************************
  Title          : fact.h
  Author         : Stewart Weiss
  Created on     : February 28, 2011
  Description    : returns n! for sufficiently small n
  Purpose        : header file for fact.c
  Build with     : 
 
******************************************************************************/

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

#include <limits.h>

/*
 * Computes factorial(n) provided that factorial(n)
 * is less than LONG_MAX, the largest signed long
 * integer, otherwise it returns -1.
*/
long fact(int n);

#endif
