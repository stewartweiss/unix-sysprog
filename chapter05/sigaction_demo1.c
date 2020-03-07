/******************************************************************************
  Title          : sigaction_demo1.c
  Author         : Stewart Weiss
  Created on     : March 24, 2011
  Description    : Demonstrates how to use siginfo_t struct
  Purpose        : To show the information in the siginfo_t struct
  Usage          : sigaction_demo1
                   and then within the next minute, type Ctrl-C
                   and run again but this time get the processid of the running
                   process and issue "kill -2 pid" where pid is its processid.
                   You need to have a second terminal window ready.
  Build with     : gcc -o sigaction_demo1  sigaction_demo1.c
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
#include <sys/types.h>
#include <signal.h>
//#include <asm-generic/siginfo.h>
#include <stdio.h>
#include <stdlib.h>

void sig_handler (int signo, siginfo_t *info,  void *context)
{
    printf ("Signal number: %d\n", info->si_signo);
    printf ("Error number:  %d\n", info->si_errno);
    printf ("PID of sender: %d\n", info->si_pid);
    printf ("UID of sender: %d\n", info->si_uid);
    exit(1);
}


int main (int argc, char* argv[])
{

   struct sigaction the_action;

   the_action.sa_flags = SA_SIGINFO;
   the_action.sa_sigaction = sig_handler;

   sigaction(SIGINT, &the_action, NULL);

   printf ("Type Ctrl-C within the next minute or send signal 2. \n");
   sleep(60);
   return 0;
}






