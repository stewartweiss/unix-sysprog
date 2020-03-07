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

#include <limits.h>

/*
 * Computes factorial(n) provided that factorial(n)
 * is less than LONG_MAX, the largest signed long
 * integer, otherwise it returns -1.
*/
long fact(int n);

#endif
