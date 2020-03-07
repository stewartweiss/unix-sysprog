/******************************************************************************
  Title          : signal_demo2.c
  Author         : Stewart Weiss
  Created on     : March 22, 2008
  Description    : Uses signal() to ignore ^C and ^\
  Purpose        : To show how to ignore signals.
  Usage          : signal_demo2  
                   While it is running, enter a few ^C and ^\ characters.
  Build with     : gcc -Wall -g -o signal_demo1
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

#include  <stdio.h>
#include  <unistd.h>
#include  <signal.h>

/*****************************************************************************/
/*                                 Main                                      */
/*****************************************************************************/
int main(int argc, char* argv[])
{
    int i;

    signal( SIGINT, SIG_IGN );   /* ignore Ctrl-C */
    signal( SIGQUIT, SIG_IGN );  /* ignore Ctrl-\ */

    printf("You can't stop me!\n");
    for(i = 20; i > 0; i-- ) {
        printf("Try to kill me with ^C or ^\\. Seconds remaining: %2d\n",
                i);
        sleep(1);
    }
    return 0;
}

