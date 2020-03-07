/******************************************************************************
  Title          : die.c
  Author         : Stewart Weiss
  Created on     : January 30, 2010
  Description    : prints error messages and then terminates calling process
  Purpose        : 
  Build with     : 
 
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
