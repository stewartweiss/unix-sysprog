/******************************************************************************
  Title          : procmaskdemo.c
  Author         : Stewart Weiss
  Created on     : March 28, 2006
  Description    : Gets orig signal mask, modifies blocked set, restores orig 
  Purpose        : To show how to create signal masks for blocking signals
  Usage          : procmaskdemo
                   This adds SIGINT to the set of blocked signals, so first do
                   not send a Ctrl-C and watch how it runs. Then send a Ctrl-C
                   before first loop ends and see how the signal is delayed. 
                   Then type the Ctrl-C in the second loop.
  Build with     : gcc -Wall -g -o procmaskdemo procmaskdemo.c
  Modifications  :

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
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    int i;
    sigset_t signals, prevsignals;

    sigemptyset(&signals);
    sigaddset(&signals, SIGINT);
    sigprocmask(SIG_BLOCK, &signals, &prevsignals);
    
    printf("SIGINT is blocked.\n");
    for ( i = 0; i < 5; i++) {
        sleep(1); 
        printf("elapsed time = %d seconds\n", i);
    }

    sigprocmask(SIG_SETMASK, &prevsignals, NULL);
    printf("Original signal set is restored.\n");

    for ( i = 0; i < 5; i++) {
         sleep(1);
         printf("elapsed time = %d seconds\n", i);
    }
    return 0;
}

