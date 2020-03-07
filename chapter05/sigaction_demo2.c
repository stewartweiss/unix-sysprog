/******************************************************************************
  Title          : sigaction_demo2.c
  Author         : Stewart Weiss
  Created on     : March 22, 2008
  Description    : Allows flags and masks of sigaction to be tested
  Purpose        : Allows user to control various settings of sigaction.
                   See the details below. It is a way to understand the
                   effects of the various signal handling flags and masks.
  Build with     : gcc -Wall -g -o sigaction_demo2 sigaction_demo2.c
  Modifications  : April 4, 2011


                   Simplified main loop to show effects of restarting more
                   easily. Changed to blocking mode only.
                   Added error checking after read system call.
       

  Usage          : 
                   sigaction_demo2  [ reset | noblock | restart ]*
 
  
   i.e., 0 or more of the words reset, noblock, and restart may appear
   on the command line, and multiple instances of the same word as the same
   effect as a single instance.
  
   reset   turns on SA_RESETHAND
   noblock turns on SA_NODEFER
   restart turns on SA_RESTART
  
   NOTES
   (1) If you supply the word "reset" on the command line it will set the
       handling to SIG_DFL for signals that arrive when the process is
       in the handler. If noblock is also set, the signal will have the default
       behavior immediately. If it is not set, the default will delay until
       after the handler exits. If noblock is set but reset is not, it will
       recursively enter the handler.
   (2) I made inthandler the handler for the SIGQUIT call too, so if it is
       not reset, neither Ctrl-C nor Ctrl-\ will kill it
   (3) It will ask you to enter the numeric values of signals to block. If
       you don't give any, no signals are added to the mask. By default the
       signal being handled is always blocked unless NODEFER is set.

   OUTPUT
   While the program is running, when you send it a signal, you will see output
   of the form
            Entered handler: sig = 2 	id = 788704
            Leaving handler: sig = 2 	id = 788704
   if you send it a Ctrl-C for example. The id is synthesized from the clock 
   time and a small integer ticker.  It blocks waiting for user input, so you
   can enter the keyboard signals while it is in the read() call.

   HOW TO USE IT
     
   Try setting each of the command line options separately at first. Without
   any options,  send three or four instances of the same signal, either SIGINT
   or SIGQUIT (^C or ^\) to the process. You will see that the signal handler is
   entered once, and exited, and then a second time and exited. You will not see
   the 3rd and 4th signals cause the handler to run. This is because only a
   single instance of a pending signal of the same type will be queued.

   Try instead to send alternating ^C and ^\ signals to the process. You will 
   see that the handler is entered first for the ^C, then the ^\, and the 
   remaining two signals are queued up. Only the first two of each will be 
   handled; the remaining ones will be lost.

   If you set noblock, and repeat the same experiment, you will instead see 
   the handler calls stacking. I.e., the sequence will be like 
         enter a; enter b. enter c; leave c; leave b; leave a.
   This shows that the SA_NODEFER flag turns off blocking of signals.

   If you set reset, then the default behavior is set just prior to the start
   of the handler, so a second Ctrl-C will kill the process, even if it is
   masked, because as soon as it is caught, the process will be killed. Try 
   masking 2 and not masking 2 with reset on. You will see that the process is
   killed whether it is masked or not.

   Ordinarily, when a system call is interrupted by a signal, it terminates
   with an error message. This is true of some, but not all calls. It is true
   of reads from the terminal.

   With restart on, these system calls will be restarted instead of terminated. 
   To see the effect of restart, first run without the flag and type Ctrl-C
   before any input, and then some characters. You will see that the read() 
   terminated with an error. You will not see the text redisplayed on the output 
   stream.    With restart set, type Ctrl-C and then some input and then the
   <ENTER>. You will see the text redisplayed because the read() call was
   restarted and read the characters from the input queue.

   With restart off, you will see the message "read call was interrupted" when
   you type a ^C or ^\.  With restart on, after the handlers run, the read() is
   restarted and waiting for you to enter text. Enter some characters and then
   ENTER and you will see them output on the screen, proving that the call was
   restarted.

   In order to see the effects of the SA_RESTART flag, blocking input must be 
   set in the file connection, otherwise the read calls will be too fast to 
   interrupt. This program will not work if the terminal is in non-blocking mode.
 
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
#include  <string.h>
#include  <signal.h>
#include  <time.h>
#include  <fcntl.h>
#include  <errno.h>
#include <termios.h>

#define	INPUTLEN	4
/*****************************************************************************/


void interrupt_handler (int signo, siginfo_t *info, void *context);

/*****************************************************************************/
/*                                    Main                                   */
/*****************************************************************************/

int main(int ac, char* av[])
{
    struct sigaction newhandler;        /* new settings        */
    sigset_t     blocked;               /* set of blocked sigs */
    void         inthandler();          /* the handler         */
    char         x[INPUTLEN+1];
    int          flags = 0;
    int          signo;
    int          n;
    char         s[] = "Entered text: ";
    int          s_len = strlen(s);

    while ( 1 < ac ) {
        if (0 ==  strncmp("reset", av[ac-1], strlen(av[ac-1])) )
            flags |= SA_RESETHAND;
        else if (0 ==  strncmp("noblock", av[ac-1], strlen(av[ac-1])) )
            flags |= SA_NODEFER;
        else if (0 ==  strncmp("restart", av[ac-1], strlen(av[ac-1])) )
            flags |= SA_RESTART;
        ac--;
    }
    /* load these two members first */
    newhandler.sa_sigaction = interrupt_handler;/* handler function    */
    newhandler.sa_flags = SA_SIGINFO | flags;   /* new style handler   */

    /* then build the list of blocked signals */
    sigemptyset(&blocked);                  /* clear all bits      */

    printf("Type the numeric value of a signal to block (0 to stop):");
    while ( 1 ) {
        scanf( "%d", &signo );
        if ( 0 == signo ) 
             break;
        sigaddset(&blocked, signo);        /* add signo to list */
        printf("next signal number (0 to stop): ");
    }

    newhandler.sa_mask = blocked;        /* store blockmask     */

    /* install inthandler as the SIGINT handler */
    if ( sigaction(SIGINT, &newhandler, NULL) == -1 )
        perror("sigaction");

    /* if successful, install inthandler as the SIGQUIT handler also */
    else if ( sigaction(SIGQUIT, &newhandler, NULL) == -1 )
        perror("sigaction");
    else {       
        while( 1 ){
            x[0] = '\0';
            tcflush(0,TCIOFLUSH);
            printf("Enter text then <RET>: (quit to quit)\n");            
            n = read(0, &x, INPUTLEN);
           if (-1 == n  &&  EINTR == errno ) {
                printf("read call was interrupted\n");
                x[n] = '\0';
                write(1, &x, n+1);
            }
            else {
                if ( strncmp("quit", x, 4) == 0 )
                    break;
                else {
                    x[n] = '\0';
                    write(1, &s, s_len);
                    write(1, &x, n+1);
                    printf("\n");
                }
            }
        }
    }
    return 0;
}

/*****************************************************************************/


void interrupt_handler (int signo, siginfo_t *info, void *context)
{
    int      localid;     /* will store a number to uniquely identify signal */
    time_t   timenow;     /* current time -- used to generate id */
    static   int ticker = 0;  /* used for id also */
    struct tm *tp;         

    time(&timenow);
    tp = localtime(&timenow);
    localid = 36000*tp->tm_hour + 600*tp->tm_min + 10*tp->tm_sec + ticker++ % 10;
    printf("Entered handler: sig = %d \tid = %d\n", info->si_signo, localid );
    sleep(3);
    printf("Leaving handler: sig = %d \tid = %d\n", info->si_signo, localid );
}
