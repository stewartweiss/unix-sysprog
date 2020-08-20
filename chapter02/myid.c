/******************************************************************************
  Title          : myid.c
  Author         : Stewart Weiss
  Created on     : January 27, 2012
  Description    : An implementation of the id command
  Purpose        : To demonstrate how to access identity-related kernel
                   databases using various parts of the API related to identity
                   including getuid(), getgid(), getpwuid(), getgrgid(), and
                   getgroups().
  Usage          : myid
  Build with     : gcc -o myid   myid.c
  Modifications  :

******************************************************************************
 * Copyright (C) 2020 - Stewart Weiss
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>


void handle_error(char *mssge)
{
    perror(mssge);
    exit(EXIT_FAILURE);
}


int main( int argc, char* argv[])
{
    uid_t           userid;
    gid_t           groupid;
    gid_t          *secondarygroups;
    int             numgroups;
    int             k;
    struct passwd  *psswd_struct;
    struct group   *group_struct;

    /* Get the real user id and real group id associated with the process
     * which is the same as that of the user who runs this command.
     */
    userid        = getuid();
    groupid       = getgid();

    /* We need also the user name and group name, which we have to get by
     * retrieving the password and group structures. We can get these
     * structures rrom the real user id and real group id using the following
     * functions.
     */
    psswd_struct  = getpwuid(userid);
    group_struct  = getgrgid(groupid);

    /* Print out the user id and group id with the names, in the same format as
     *  id.
     */
    printf( "uid=%d(%s) gid=%d(%s) ", userid, psswd_struct->pw_name,
                                    groupid, group_struct->gr_name);

    /* To get the list of secondary groups, we use the getgroups() function.
     * When getgroups() first argument is zero, it returns the number of
     * elements in the list that it would fill as its second argument if the
     * first were not zero. By first calling it with a zero first argument,
     * we can get the list size. We then dynamically allocate an array to use
     * in a second call.
     */
    numgroups = getgroups(0, secondarygroups);
    if ( numgroups < 0 )
        handle_error("Trying to get secondary groups");

    if ( numgroups > 0 ) {
        secondarygroups = calloc( numgroups, sizeof(gid_t) );
        if ( NULL == secondarygroups ) {
            handle_error("Trying to allocate array for secondary groups");
        }
        if ( -1 == getgroups( numgroups, secondarygroups )) {
            handle_error("Trying to get secondary groups");
        }
    }

    /* If the number of groups is at least one, print out a list of them.
       We use getgrgid() to get the group structure for each group id in the
       list created by getgroups, and get the name from that structure.
       We print a comma after all names except the last.
     */
    if ( numgroups > 0 ) {
        printf("groups=");
        for (k = 0; k < numgroups; k++ ) {
            group_struct   = getgrgid(secondarygroups[k]);
            printf("%d(%s)", secondarygroups[k], group_struct->gr_name);
            if ( k < numgroups-1 )
                printf(",");
        }
        free(secondarygroups);
    }
    printf("\n");
    return 0;

}
