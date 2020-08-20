/******************************************************************************
  Title          : cp3.c
  Author         : Stewart Weiss
  Created on     : February 6, 2013
  Description    : Does a file to file copy using memory-mapped I/O
  Purpose        : To demonstrate how to use memory-mapped I/O and to show
                   how much faster it can be in certain circumstances.
  Build with     : gcc -o cp3 cp3.c  ../utilities/die.c

  Notes
  Ordinary I/O functions such as the read() and write() system calls use the
  kernel's buffering system to perform transfers between memory and the device.
  This means that on a read, for example, data will first be transferred to a
  kernel buffer and then to the user space memory.

  Modern Unix systems, such as Linux, usually provide a means of turning off
  the kernel buffering, such as with the use of the O_SYNC flag or its variants,
  but this is often problematic, either because it is not implemented fully,
  or not guaranteed to write memory contents immediately, or because certain
  file systems do not support it.

  Memory mapping is a way to perform I/O without kernel buffering, and it is
  fully supported on almost all systems. The concept has been around for a long
  time.  The idea in its simplest form is easy to understand: a process can
  request that a file be mapped to a set of virtual memory addresses. Changes
  to those addresses are, in effect, writes to the file. Reads of those
  addresses are reads of the file.

  The actual use of the memory mapping system calls, mmap() and munmap(), is a
  bit more complex than this. The purpose of munmap(), as its name suggests,
  is to undo a mapping. The mmap() call has several parameters. This first
  program uses a very simple mapping of the input file to memory, to ease into
  this topic.

  A caveat -- the documentation on my Linux system states that mmap() has been
  deprecated in favor of mmap2(), but mmap2() does not exist on it. In fact,
  glibc (GNU's C Standard Library) implements mmap() as a wrapper for the
  kernel's mmap2() call, so mmap() is actually mmap2().  This is why I use
  mmap() here and not mmap2().

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


#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include "../utilities/die.h"

#define COPYMODE        0666


int main(int argc, char *argv[])
{
    int     in_fd, out_fd;
    size_t  filesize;
    char    nullbyte;
    void    *source_addr;
    void    *dest_addr;

    /* check args 	*/
    if ( argc != 3 ){
        fprintf( stderr, "usage: %s source destination\n", *argv);
        exit(1);
    }

	  /* open files	*/
    if ( (in_fd = open(argv[1], O_RDONLY)) == -1 )
        die("Cannot open ", argv[1]);

    /* The file to be created must be opened in read/write mode because of how
       mmap()'s PROT_WRITE works on i386 architectures. According to the man
       page, on some hardware architectures (e.g., i386), PROT_WRITE implies
       PROT_READ.  Therefore, setting the protection flag to PROT_WRITE is
       equivalent to setting it to PROT_WRITE|PROT_READ if the machine
       architecture is i386 or the like. Since this flag has to match the flags
       by which the mapped file was opened, I set the opening flags differently
       for the i386 architecture than for others.
    */
#if defined (i386) || defined (__x86_64) || defined (__x86_64__) \
       || defined (i686)
    if ( (out_fd = open( argv[2], O_RDWR | O_CREAT | O_TRUNC, COPYMODE )) == -1 )
        die( "Cannot create ", argv[2]);
#else
    if ( (out_fd = open( argv[2], O_WRONLY | O_CREAT | O_TRUNC, COPYMODE )) == -1 )
        die( "Cannot create ", argv[2]);
#endif
    /* get the size of the source file by seeking to the end of it:
       lseek() returns the offset location of the file pointer after the seek
       relative to the beginning of the file, so this is a good way to get
       an opened file's size.
    */
    filesize = lseek(in_fd, 0, SEEK_END);

    /* By seeking to filesize in the new file, the file can be grown to that
       size. Its size does not change until a write occurs there. */
    lseek(out_fd, filesize-1, SEEK_SET);

    /* So we write the NULL byte and the file size is now set to filesize. */
    write(out_fd, &nullbyte, 1);

    /* Time to set up the memory maps */
    if ( ( source_addr = mmap(NULL, filesize, PROT_READ,
               MAP_SHARED, in_fd, 0) ) == (void *) -1 )
        die( "Error mapping file ", argv[1]);

    if ( ( dest_addr = mmap(NULL, filesize, PROT_WRITE,
               MAP_SHARED, out_fd, 0) ) == (void *) -1 )
        die( "Error mapping file ", argv[2]);

    /* copy the input to output by doing a memcpy */
    memcpy( dest_addr, source_addr, filesize );

    /* unmap the files */
    munmap(source_addr, filesize);
    munmap(dest_addr, filesize);

    /* close the files */
    close(in_fd);
    close(out_fd);

    return 0;

}
