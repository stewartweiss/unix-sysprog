/******************************************************************************
  Title          : show_utmp.c
  Author         : Stewart Weiss, based on [Molay]
  Created on     : February, 2006
  Description    : Demonstrates how to process utmp structures
  Purpose        : 
  Usage          : show_utmp [wtmp]
                   if wtmp argument supplied, it shows the contents of
                   wtmp file, otherwise utmp file
  Build with     : gcc -o show_utmp show_utmp.c -DSHOWHOST -I../include \
                   -L../lib -lutils  

******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <utmp.h>
#include <fcntl.h>
#include <string.h>
#include "utils.h"


/*****************************************************************************
  show info( struct utmp* )
  displays contents of the utmp struct in human readable form
 *****************************************************************************/
void show_info(struct utmp *);


/*****************************************************************************
  show type( int )
  displays string representing integer value of utmp type
 *****************************************************************************/
void show_type(int );


/*****************************************************************************
                               Main Program
*****************************************************************************/
int main(int argc, char* argv[])
{
    struct utmp     utbuf;          /* read info into here */
    int             utmpfd;         /* read from this descriptor */
 
    if ( (argc > 1) && (strcmp(argv[1],"wtmp") == 0) ) {
        if ( (utmpfd = open(WTMP_FILE, O_RDONLY)) == -1 ){
            perror(WTMP_FILE);
            exit(1);
        }
    }
        
    else 
        if ( (utmpfd = open(UTMP_FILE, O_RDONLY)) == -1 ){
            perror(UTMP_FILE);
            exit(1);
        }

    while( read(utmpfd, &utbuf, sizeof(utbuf)) == sizeof(utbuf) )
        show_info( &utbuf );
    close(utmpfd);
    return 0;
}


/*****************************************************************************
  show info()
  displays contents of the utmp struct in human readable form
  The sizes used in the printf below are not guaranteed to work on all systems.
  The ut_time member may be 32 or 64 bits.
  If the type of entry is not USER_PROCESS, it skips the record
 *****************************************************************************/
void show_info( struct utmp *utbufp )
{
    show_type(utbufp->ut_type);
    printf("%-8.8s", utbufp->ut_name);      /* the logname  */
    printf(" ");                            /* a space      */
    printf("%-8.8s", utbufp->ut_line);      /* the tty      */
    printf(" ");                            /* a space      */
    show_time( utbufp->ut_time );           /* display time */
    printf(" ");                           
    printf("%-3d ", utbufp->ut_exit.e_exit);
    printf("%-3d ", utbufp->ut_exit.e_termination);

#ifdef SHOWHOST
    if ( utbufp->ut_host[0] != '\0' )
        printf(" (%s)", utbufp->ut_host);/* the host    */
#endif
        printf("\n");                   
}


/*****************************************************************************
  show type( int )
  displays string representing integer value of utmp type
 *****************************************************************************/
void show_type( int t)
{
    switch (t)
    {
    case RUN_LVL:      	printf("RUN_LVL       "); break;               
    case BOOT_TIME:     printf("BOOT_TIME     "); break;             
    case NEW_TIME:      printf("NEW_TIME      "); break;              
    case OLD_TIME:      printf("OLD_TIME      "); break;              
    case INIT_PROCESS:  printf("INIT_PROCESS  "); break;          
    case LOGIN_PROCESS: printf("LOGIN_PROCESS "); break;         
    case USER_PROCESS:  printf("USER_PROCESS  "); break;          
    case DEAD_PROCESS:  printf("DEAD_PROCESS  "); break;          
    case ACCOUNTING:    printf("ACCOUNTING    "); break;            
    }
}
