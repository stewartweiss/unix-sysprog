/******************************************************************************
  Title          : signal_demo1.c
  Author         : Stewart Weiss
  Created on     : March 22, 2008
  Description    : Installs signal handlers for ^C and ^\
  Purpose        : To show how to install signals and the effect of the handlers
  Usage          : signal_demo1
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
/*                                 Handlers                                  */
/*****************************************************************************/
void catch1(int signum)
{
    printf("You can do better than that!\n");
}

void catch2(int signum)
{
    printf("I'm no quitter!\n");
}

/*****************************************************************************/
/*                                    Main                                   */
/*****************************************************************************/

int main(int argc, char* argv[])
{
    int	i;

    signal( SIGINT, catch1 );
    signal( SIGQUIT,catch2 );
    for(i = 20; i > 0; i-- ) {
        printf("Try to kill me with ^C or ^\\. Seconds remaining: %2d\n",
                i);
        sleep(1);
    }
    return 0;
}


