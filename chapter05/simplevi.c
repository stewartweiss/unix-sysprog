/******************************************************************************
  Title          : simplevi.c
  Author         : Stewart Weiss
  Created on     : April 3, 2013
  Description    : A very simple visual editor based on vi
  Purpose        : To demonstrate some of the principles used to implement
                   a character-terminal-based editor like vi, without using
                   the Curses library.
  Usage          : simplevi
                   See below for detailed usage instructions
  Build with     : gcc -Wall -g -o simplevi simplevi.c -L../lib  -lutils -lm


USAGE and LIMITATIONS
The purpose of this demo program is to illustrate some of the principles
underlying a visual editor in a character-based display such as a pseudo-
terminal window.  It has very limited functionality; the most serious 
omission is the ability to delete entered text. It can only be used for 
creating text, without making any mistakes.

USAGE

This program allows a user to create a file in a manner similar to vi. It 
has only an insert command and does not presently support deletion.
Like vi, it has three modes in which it can be:
command_mode
input_mode
lastline_mode

Command Mode
The initial mode is command mode. In command_mode one can enter the following
keystrokes/commands:
i              changes to input mode.
:              changes to lastline mode
Navigation Keys:  
               move the cursor in the indicated direction, using rules 
               similar, but not identical, to vi's rules. The rules are as
               follows:
'l', spacebar, or the right-arrow key: 
               moves right until the last character in the text line. If a 
               line wraps, the cursor follows the wrapped text.
'h', backspace, or left-arrow key:  
               moves left until the first character in the text line. If a
               line wraps, it follows the wrapped text backwards.
'k' or up-arrow key:    
               moves to the text line above, to the same position relative to
               the start of the text line, unless that text line is shorter
               than the current one, in which case it is placed at the end of
               the text line. Scrolling takes place as necessary.
'j' or down-arrow key:  
               moves to the next text line below, using the same rule as for
               the up arrow when deciding the horizontal position. Scrolling 
               takes place as necessary.
Note: 
Because there is only an insert operation, and not an append operation, the
cursor is allowed to be one position to the right of the rightmost character
in a line.


Ctrl-D         These do nothing but show how they are disabled from causing
Ctrl-C         terminal signals.
Ctrl-H         Can be used to display a help screen, but at present just
               shows a one-line text message.

Lastline Mode
The user can quit the editor in lastline mode, and/or save the current
buffer contents. The allowed commands are a subset of the commands in vi,
but the syntax mimics vi. Specifically, in lastline mode one can enter:
w  filename  to save the buffer to a file named filename
wq filename  to save the buffer to a file named filename and quit after
q  to quit without saving. It does not warn that the buffer is not saved.

Any amount of white space is allowed before the command and between the 
command and the filename. Any characters other than these generate an
error message and terminate lastline mode.

Filenames can be any combination of alphanumeric characters and underscores.
Punctuation and whitepace are not allowed.  Typing a command like this:
: w  q
creates a file named 'q', as would happen in vi.

Letting S denote a space character, F denote a valid filename character, 
parentheses for grouping, and | for alternation, the language of acceptable 
newline-terminated strings in lastline mode is
S*(w|wS*q)SS*FF*S* | S*qS*



Input Mode
There is only an insert operation, which inserts at the cursor position.
The backspace is not implemented. Typing a backspace has no effect. Typing
characters other than graphical characters (those found on the keyboard) has
unspecified behavior. Graphical characters are inserted to the left of
the cursor. Lines wrap as necessary, and the screen scrolls as needed as
well.



Design 
The program manages the following objects:
the text buffer,
the cursor, and
the window.

The primary work of the application is to synchronize the buffer contents,
the cursor, and the window. Certain cursor movements cause the visible portion
of the buffer to be scrolled upward or downward. Insertions require that the
buffer be redrawn on the window after each character, and may also cause
scrolling.

Status messages of various kinds are written to the last line of the window,
and buffer contents are never allowed to be drawn in that line. Therefore,
the application has to save the cursor position whenever a write to the
last line must take place and return to the previous position after the 
write.

For simplicity and not performance, the text buffer is a linear array of 
characters. Each insertion causes the entire array to the right of the 
insertion to be shifted upwards. 
Because vi is a line-oriented editor and cursor movement follows text lines,
it is convenient for the text buffer to keep track of the line starts and
lengths, as well as the current line in which the cursor is located and
its offset from the beginning of that line.  It also stores the offset
from the beginning of the linear array itself, and the number of lines
in the buffer.  It has functions to synchronize all members.

The window maintains its dimensions and the number of the text buffer line
currently at the top of the window. This enables the aapplication to 
determine whether scrolling is necessary and also to find the position within
within the text buffer of the cursor.

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


#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <termios.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <getopt.h>
#ifndef TIOCGWINSZ
#include <sys/ioctl.h>
#endif

#define RETRIEVE      1                 
#define RESTORE       2                
#define FALSE         0
#define TRUE          1
#define ESCAPE       '\033'
#define CONTROL_C    '\003'
#define CONTROL_D    '\004'
#define CONTROL_H    '\010'
#define KEY_UP       65
#define KEY_DOWN     66
#define KEY_RIGHT    67
#define KEY_LEFT     68
#define MAXLINES     1000
#define MAXCHARS     255
#define OUT_OF_LINES -1
#define OUT_OF_MEM   -2
#define UNHANDLEDCHAR -3

#define CLEARANDHOME  write(1,CLEAR_SCREEN, lCLEAR_SCREEN);\
                      write(1,CURSOR_HOME, lCURSOR_HOME);


/* ANSI Escape Sequences and their lengths (for speed */
const char CLEAR_DOWN[]   = "\033[0J";
const int  lCLEAR_DOWN    =  4;
const char CLEAR_RIGHT[]  = "\033[0K";
const int  lCLEAR_RIGHT   =  4;
const char CURSOR_HOME[]  = "\033[1;1H";
const int  lCURSOR_HOME   =  6;
const char CLEAR_SCREEN[] = "\033[2J";
const int  lCLEAR_SCREEN  =  4;
const char CLEAR_LINE[]   = "\033[2K";
const int  lCLEAR_LINE    =  4;
const char RIGHT[]        = "\033[1C";
const int  lRIGHT         =  4;
const char LEFT[]         = "\033[1D";
const int  lLEFT          =  4;
const char BACKSPACE[]    = "\033[1D \033[1D";
const int  lBACKSPACE     =  9;
/* The following cannot be declared statically as the dimensions are unknown */
char       PARK[20];      /* string to park cursor at lower left */
int        lPARK;         /* length of PARK string */

/* Miscellaneous strings for output */
const char CTRLC[]        = "You typed Control-C.";
const char CTRLD[]        = "You typed Control-D.";
const char CTRLH[]        = "This is the Help Command. Not much help, sorry!";
const char BLANK          = ' ';
const char INSERT[]       = "--INSERT--";
const int  lINSERT        = 10;
const char MAXLINES_MSSGE[] 
                          = "You reached the maximum number of lines."
                            " Exiting input mode.";
const char OUT_OF_MEM_MSSGE[] 
                          = "You reached the maximum buffer size."
                            " Exiting input mode.";
const char UNHANDLEDCHAR_MSSGE[] 
                          = "This input not yet implemented."
                            " Exiting input mode.";

void clearandhome()
{
    write(1,CLEAR_SCREEN, lCLEAR_SCREEN);
    write(1,CURSOR_HOME, lCURSOR_HOME);
}

void park()
{
    write(1,PARK, lPARK);
    write(1, CLEAR_LINE, lCLEAR_LINE);
}

/*****************************************************************************/
/*                                Data Types                                 */
/*****************************************************************************/


/**
 * A Cursor stores the coordinates of the position of the cursor on the
 * visible screen. It is 1-based: (1,1) is the upper left-hand corner.
 */
typedef struct _cursor
{
   int r;
   int c;
} Cursor;

/** 
   A Window structure encapsulates the dimensions of the window and the
   amount by which the text within it has been scrolled. The  line_at_top
   member is the lowest index of all lines in the text buffer that is currently
   visible in the window, i.e., the index of the top line on the screen.
   Only complete lines are displayed, so line_at_top is the index of the
   line whose entire contents are visible. The erase character, erase_char,
   is the character for the given terminal that is mapped to C_CC[VERASE].
*/
typedef struct _window
{
   unsigned short rows;
   unsigned short cols;
   int            line_at_top;
   char           erase_char;
} Window;

/**
   A Buffer structure encapsulates the actual text data. It contains a linear
   array of characters and an array that indicates how long each text line is.
   It also has members that store how many lines of text it has (num_lines), 
   how many characters (size), and the location of the cursor within the 
   text, represented in two different ways:
   index -- the offset from the beginning of the text buffer, and
   (cur_line, index_in_cur_line) -- the current line and the index within that
                                    line.
*/
typedef struct _buffer
{
    char text[BUFSIZ];
    int  line_len[MAXLINES]; /* lengths of text lines, including newline
                                characters */
    int  line_start[MAXLINES]; /* starts of each line */
    int  num_lines;          /* number of text lines in buffer. This 
                                includes lines that have not yet been
                                terminated with a newline character. It is
                                the number of newline characters + 1 if the
                                last character in the buffer is not a 
                                newline. */
    int  index;              /* index of cursor in text buffer */
    int  size;               /* total chars in buffer */
    int  cur_line;           /* current text line, not screen line */
    int  index_in_cur_line;  /* index in current line of cursor */
} Buffer;

/*****************************************************************************/
/*                           Function Prototypes                             */
/*****************************************************************************/



/* Window/Terminal Functions */
void init_window         ( int fd,  Window *win );
void moveto              ( int line, int column );
void write_status_message( const char *message, Cursor curs );
void save_restore_tty(int fd, int action);
void modify_termios( int fd, int echo, int canon);
void set_erase_char(int termfd, Window *win );



/* Buffer Functions */
int  insert( Buffer *buf, Window win, char ch );
void init_buffer( Buffer *buffer);
void update_buffer_index( Buffer *buffer );
int  buffer_index( int index_in_line, int cur_line, int linelength[] );
void redraw_buffer( Buffer buffer,  Window *win, Cursor *curs ); 
void scroll_buffer( Buffer buf, Window win );
int  line_in_buffer( Buffer buf, Window win, int pos );
void save_buffer ( const char path[], Buffer buf, char *statusstr );
int  handle_insertion( Buffer *buf, Window *win, Cursor *curs, char c);
/** get_lastline_in_win()
 *  This calculates the number of complete text lines that can be displayed
 *  in the window and returns index of last complete line.
 */
void get_lastline_in_win( Buffer buffer, Window win,  int *lastline );

/* Lastline Mode */
int parselastline( char *str, int len, Buffer buf, char *statusstr );
int do_lastline_mode(Buffer buf, Window win, Cursor curs );




/* Cursor Functions */
void init_cursor   ( Cursor *cursor );
void show_cursor   ( Buffer buf, Window win, Cursor cursor, 
                     int index_in_line, int line_number );
void advance_cursor( Cursor *cursor, Window win, char ch );
void get_cursor_at (Buffer buf, Window win, int index, 
                   int lineno, Cursor *curs);
void handle_escape_char( Buffer *bin, Window *win, Cursor *curs );
void moveUp   ( Buffer *buf, Window *win, Cursor *curs );
void moveDown ( Buffer *buf, Window *win, Cursor *curs );
void moveRight( Buffer *buf, Window *win, Cursor *curs );
void moveLeft ( Buffer *buf, Window *win, Cursor *curs );


/*****************************************************************************/
/*                                    Main                                   */
/*****************************************************************************/

int main(int argc, char* argv[])
{
    int     quit             = 0;
    int     in_input_mode    = 0;   
    //int     in_lastline_mode;  
    Buffer  buf;
    Window  win;
    Cursor  curs;                /* cursor position (0,0) is upper left */
    char    prompt = ':';        /* prompt character */       
    char    c;
    int status;

    /* Check if either input or output is redirected and exit if so */
    if ( !isatty(STDIN_FILENO) || !isatty(STDOUT_FILENO) ) {
        fprintf(stderr, "Not a terminal\n");
        exit(1);
    }

    /* Save the original tty state */
    save_restore_tty(STDIN_FILENO, RETRIEVE);  

    /* Modify the terminal - turn off echo, keybd sigs, and canonical mode */
    modify_termios( STDIN_FILENO, 0, 0);

    /* Initialize the buffer, the cursor, and the window. */
    init_buffer(&buf);
    init_cursor(&curs);
    init_window(STDIN_FILENO, &win);

    /* Create a string to park cursor and save its length for speed. */
    sprintf(PARK, "\033[%d;1H", win.rows);
    lPARK = strlen(PARK);

    /* Clear the screen and put cursor in upper left corner */
    clearandhome();

    while ( !quit ) {
        if ( in_input_mode ) {
            if (  read(STDIN_FILENO, &c, 1) > 0  ) {
                if ( c == ESCAPE ) {
                    in_input_mode = 0;
                    write_status_message( "          ", curs);
                }
                else {
                    /* insert typed char and echo it */
                    in_input_mode = handle_insertion( &buf, &win, &curs, c);   
                    if ( in_input_mode == UNHANDLEDCHAR )
                        in_input_mode = 1;
                    else
                        write_status_message( INSERT, curs);
                }
            }
        }
        else {
            if ( read(STDIN_FILENO, &c, 1) > 0  ) {
                switch ( c ) {
                case 'i':
                    in_input_mode = 1;
                    park();
                    update_buffer_index(&buf);
                    moveto(curs.r, curs.c);
                    write_status_message( INSERT, curs);
                    break;
                case ':':
                    //in_lastline_mode = 1;
                    park();
                    write(1,&prompt, 1);
                    status = do_lastline_mode( buf, win, curs);
                    if ( status >= 0 ) quit = status;
                    moveto(curs.r, curs.c);
                    break;
                case '\003':
                    write_status_message( CTRLC, curs);
                    break;
                case '\004':
                    write_status_message( CTRLD, curs);
                   break;
                case '\010':
                    write_status_message( CTRLH, curs);
                    break;
                case 'j':
                    moveDown(&buf, &win, &curs );
                    break;
                case 'k':
                    moveUp(&buf, &win, &curs );
                    break;
                case 'l':
                case ' ':
                    moveRight(&buf, &win, &curs );
                    break;
                case 'h':
                    moveLeft(&buf, &win, &curs );
                    break;
                case ESCAPE:
                    handle_escape_char( &buf, &win, &curs);
                    break;
                default:
                    if ( c == win.erase_char )
                        moveLeft(&buf, &win, &curs );
                }
            }
        }
    }

    printf("\n");
    tcflush(STDIN_FILENO,TCIFLUSH);   
    clearandhome();
    save_restore_tty(STDIN_FILENO, RESTORE);			
    return 0;
}

/*****************************************************************************/

int parselastline( char *str, int len, Buffer buf, char *statusstr  )
{
    int i          = 0;
    int foundquit  = 0;
    int foundwrite = 0;
    //int badchar       ;
    int done       = 0;
    char *filename = NULL;
    int state;

    state = 1;
    while ( ( i < len) && !done ) {
        switch ( state ) {
        case 1:
            if ( str[i] == ' ' ) 
                state = 1;
            else if ( str[i] == 'w' ) {
                foundwrite = 1;
                state = 2;
            }
            else if ( str[i] == 'q' ) {
                foundquit = 1;
                state = 7;
            }
            else
                state = 5;
            i++;
            break;
        case 2:
            if ( str[i] == 'q' ) {
                foundquit = 1;
                state = 3;
            }
            else if ( str[i] == ' ' )
                state = 4;
            else
                state = 5;
            i++;
            break;
        case 3:
            if ( str[i] == ' ' )
                state = 4;
            else 
                state = 5;
            i++;
            break;
        case 4:
            if ( str[i] == ' ' ) 
                state = 4;
            else if ( isalnum(str[i]) || str[i] == '_' ) {
                filename = &(str[i]);
                state = 6;
            }
            else
                state = 5;
            i++;
            break;
        case 5:
           // badchar = 1;
            sprintf(statusstr, "\033[7m: %s Not an editor command\033[27m", str);
            return -1;
        case 6:
            filename = &(str[i-1]);
            while ( ( i < len) && (isalnum(str[i]) || str[i] == '_' ) )
                i++;
            str[i] = '\0';
            done = 1;
            break;
        case 7:
            if ( str[i] == ' ' ) 
                state = 7;
            else {
                //badchar = 1;
                sprintf(statusstr, 
                        "\033[7m: %s Not an editor command\033[27m", str);
                return -1;
            }
            i++;
        }
    }
    if ( foundwrite ) {
        if ( filename != NULL ) 
            save_buffer( filename, buf, statusstr );
        else {
            sprintf(statusstr, 
                    "\033[7m: %s Not an editor command\033[27m", str);
            return -1;
        }
    }
    if ( foundquit )
        return 1;
    else {
        return 0;
    }
}

/*****************************************************************************/

int do_lastline_mode(Buffer buf, Window win, Cursor curs )
{
    char tempstr[MAXCHARS];
    char statusstr[MAXCHARS];
    char c;
    int i = 0;
    int status;
    int in_lastline_mode = 1;

    while ( in_lastline_mode ) {
        read(STDIN_FILENO, &c, 1);
        if ( c == '\n' ) {
            tempstr[i] = '\0';
            status = parselastline(tempstr, strlen(tempstr), 
                                 buf, statusstr);
            in_lastline_mode = 0;
            write_status_message( statusstr, curs);
            statusstr[0] = '\0';
        }
        else if ( c == win.erase_char ) {
            write(1,BACKSPACE, lBACKSPACE );
            if ( i > 0 ) 
                i--;
            else 
                in_lastline_mode = 0;
        }
        else {
            tempstr[i++] = c;
            write(1,&c,1);
        }
    }
    return status;
}

/*****************************************************************************/

void handle_escape_char( Buffer *buf, Window *win, Cursor *curs )
{
    char c;

    read(STDIN_FILENO, &c, 1);
    if (  c  == 91 ) {
        read(STDIN_FILENO, &c, 1);
        switch ( c ) {
        case KEY_UP:
            moveUp(buf, win, curs );
            break;
        case KEY_DOWN:
            moveDown(buf, win, curs );
            break;
        case KEY_RIGHT:
            moveRight(buf, win, curs );
            break;
        case KEY_LEFT: 
            moveLeft(buf, win, curs );
            break;
        }
    }
}


void set_erase_char(int termfd, Window *win )
{
    struct termios cur_tty;
    tcgetattr(termfd, &cur_tty);

    win->erase_char = cur_tty.c_cc[VERASE];
}

/******************************************************************************/


void modify_termios(int fd, int echo, int canon )
{
    struct termios cur_tty;
    tcgetattr(fd, &cur_tty);


    if ( canon )
        cur_tty.c_lflag     |= ICANON;	
    else 
        cur_tty.c_lflag     &= ~ICANON;	
    if ( echo )
        cur_tty.c_lflag     |= ECHO;	
    else 
        cur_tty.c_lflag     &= ( ~ECHO & ~ECHOE);	

    cur_tty.c_lflag     &= ~ISIG;
    cur_tty.c_cc[VMIN]   =  1;		
    cur_tty.c_cc[VTIME]  =  0;

    tcsetattr(fd, TCSADRAIN, &cur_tty); 
}

/******************************************************************************/

void save_restore_tty(int fd, int action)
{
    static struct termios original_state;
    static int            retrieved = FALSE;

    if ( RETRIEVE == action ){
        retrieved = TRUE;
        tcgetattr(fd, &original_state);
    }
    else if (retrieved &&  RESTORE == action ) {
        tcsetattr(fd, TCSADRAIN, &original_state); 
    }
    else
        fprintf(stderr, "Illegal action to save_restore_tty().\n");
}


/*****************************************************************************
                               Window Functions
*****************************************************************************/


void  init_window( int fd, Window *win )
{
    struct winsize size;
 
    if (ioctl(fd, TIOCGWINSZ,  &size) < 0) {
        perror("TIOCGWINSZ error");
        return;
    }
    win->rows = size.ws_row;
    win->cols = size.ws_col;
    win->line_at_top = 0;
    set_erase_char( fd, win);
}

/*****************************************************************************/


void write_status_message( const char *message, Cursor curs )
{
    write(1,PARK, lPARK);
    write(1, CLEAR_LINE, lCLEAR_LINE);
    write(1, message, strlen(message));
    moveto(curs.r, curs.c);
}

/*****************************************************************************/

void moveto(int line, int column )
{
    char seq_str[20];

    sprintf(seq_str, "\033[%d;%dH", line+1, column+1);
    write(1, seq_str, strlen(seq_str));
}



/*****************************************************************************
                               Buffer Functions
*****************************************************************************/

void init_buffer( Buffer *buffer)
{
    buffer->num_lines         = 0;      
    buffer->cur_line          = 0;
    buffer->line_len[0]       = 0;
    buffer->line_start[0]     = 0;
    buffer->size              = 0;
    buffer->index_in_cur_line = 0;
    buffer->index             = 0;
}

/*****************************************************************************/

void save_buffer ( const char path[], Buffer buf, char *statusstr )
{
    char newline = '\n';
    int  fd;
    //char statusstr[80];

    fd = creat(path, 0644);
    if ( fd != -1 ) {
        sprintf(statusstr, 
          "\"%s\" %dL %dC written",
          path, buf.num_lines+1, buf.size);
        write(fd, buf.text, strlen(buf.text));
        if ( buf.text[buf.size-1] != '\n' )
            write(fd, &newline, 1);
        close(fd);
    }
    else
        exit(1);
}

/******************************************************************************/
/*
   This resets the index member of the buffer so that it is in synch
   with the (cur_line, index_in_cur_line) representation of the cursor.
*/
void update_buffer_index( Buffer *buffer ) 
{
    int totalchars = 0;
    int i = 0;
    
    while (  i < buffer->cur_line ) {
        totalchars += buffer->line_len[i];
        i++;   
    }
    totalchars += buffer->index_in_cur_line;
    buffer->index = totalchars;
}

/******************************************************************************/
/*
   This calculates the number of complete text lines that can be displayed
   in the window and returns index of last complete line.
*/
void get_lastline_in_win( Buffer buffer, Window win,  int *lastline )
{
    int totallines = 0;
    int i;
    int max_possible = win.rows-1; /* rows less status line */

    i = win.line_at_top;
    while ( i < buffer.num_lines ) {
        if ( buffer.line_len[i] <= win.cols )
            totallines++;
        else
            totallines += (int) ceil((double)buffer.line_len[i]/win.cols);
        if ( totallines > max_possible ) 
            break;
        else {
            i++;
        }
    }
    // total > max_possible, so we use previous line, which is i-1.
    *lastline = i-1;
}

/******************************************************************************/

void redraw_buffer( Buffer buffer, Window *win, Cursor *curs )
{
    int i;
    int lastline;
    int lastchar;
    int firstchar;
    int line_of_cursor;

    /*
    If the current position in the buffer, buffer.index, is not within the 
    visible lines of the window, the window must be shifted.  The shift might 
    be up or down, depending on whether index is above or below the window.

    We first need to get the number of the line containing pos. Then we 
    check whether that line is between win.line_at_top and lastline.
    We need to calculate the difference and  shift win.line_at_top that
    difference, and recalculate lastline, after which we can draw the
    buffer.
    */

    /* Compute the last visible complete text line in the buffer */
    get_lastline_in_win( buffer, *win,  &lastline );

    /* Get the index of the text line containing the insertion position */
    line_of_cursor = line_in_buffer( buffer, *win, buffer.index ) ;
    
    /* Check if the window needs to be scrolled */
    if (line_of_cursor < win->line_at_top ) {
        lastline -= (win->line_at_top - line_of_cursor);
        curs->r += (win->line_at_top - line_of_cursor);
        win->line_at_top = line_of_cursor;
    }
    else if (line_of_cursor > lastline ) {
        win->line_at_top += (line_of_cursor - lastline);
        curs->r -= (line_of_cursor - lastline);
        lastline = line_of_cursor;
    }

    /* Get the first and last characters of the visible screen. The lastchar 
       is the index of the last character that can appear in the 
       window -- the last character in the last visible line. The first char
       is the start of the line at the top of the screen. */
    lastchar  = buffer.line_start[lastline] + buffer.line_len[lastline];
    firstchar = buffer.line_start[win->line_at_top];

    /* Prepare to redraw the window. First clear the screen. */
    write(1,CLEAR_SCREEN, lCLEAR_SCREEN);

    /* Do the redraw */
    moveto(0,0);
    for ( i = firstchar; i < lastchar; i++ )
        write(1,&buffer.text[i], 1 );    
}

/******************************************************************************/

void scroll_buffer( Buffer buf, Window win )
{
    /* 
       This calculates the position of the first character on the screen
       as the leftmost character in the current line_at_top, and then
       calls get_lastline_in_win() to get the index of the last text line
       that can fit in its entirety within the window. It then computes
       the index of the last character in that line.

       It then clears the screen and writes the contents of the text buffer, 
       starting at the computed firstchar until the lastchar. The cursor
       has to be moved to the upper left-hand corner before starting.
       The caller is responsible for restoring the previous cursor position.
    */

    int i;
    int lastline;
    int lastchar;
    int firstchar = buf.line_start[win.line_at_top];

    get_lastline_in_win( buf, win,  &lastline );
    lastchar = buf.line_start[lastline] + buf.line_len[lastline];

    write(1,CLEAR_SCREEN, lCLEAR_SCREEN);
    moveto(0,0);
    for ( i = firstchar; i < lastchar; i++ )
        write(1,&buf.text[i], 1 );
}

/******************************************************************************/

/* Could use binary search instead */
int line_in_buffer( Buffer buf, Window win, int pos ) 
{
    int i = 0;
    
    while ( i < buf.num_lines ) 
        if ( buf.line_start[i] <= pos )
            i++;
        else
            break;
    /* If the inserted character is a newline, add 
       the extra line */
    if ( ( buf.text[pos] == '\n' )  )
        i++;
    return i-1;
}

/******************************************************************************/
int insert( Buffer *buf, Window win, char c )
{
    int i;

    if ( ( c == '\n' ) && ( MAXLINES == buf->num_lines ) )
        return OUT_OF_LINES;
    else if ( buf->size == BUFSIZ )
        return OUT_OF_MEM;

    if ( c == win.erase_char  ) {
        /* Not implemented -- just ignore it */
        return UNHANDLEDCHAR;
    }

    for ( i = buf->size; i > buf->index; i-- )
        buf->text[i] = buf->text[i-1];
    buf->text[buf->index] = c;
    buf->size++;
    buf->index++;
    buf->line_len[buf->cur_line]++;

    /* the first character sets line count to 1 */
    if ( buf->size == 1 ) buf->num_lines++;

    if ( c == '\n' ) {
        /* Save the length of the line being split by the newline */
        int temp = buf->line_len[buf->cur_line];

       /* The new length of current line is the current index position + 1. */
        buf->line_len[buf->cur_line] = buf->index_in_cur_line + 1;

        /* Increase number of lines */
        buf->num_lines++;  
   
        /* Shift all line starts and lengths upwards in the array, but
           add 1 to the line starts since they are 1 character further
           than before because of the new newline. Do this from the last line
           down to cur_line+1, which is the line just after the split
           line. */
        for ( i = buf->num_lines-1; i > buf->cur_line+1; i-- ) {
            buf->line_len[i] = buf->line_len[i-1];
            buf->line_start[i] = buf->line_start[i-1]+1;
        }
        /* Set the start of the new line created here. It is the sum of the
           start of cur_line plus the length of cur_line. */
        buf->line_start[buf->cur_line+1] =  buf->line_start[buf->cur_line] 
                            + buf->line_len[buf->cur_line];

        /* advance to new line */
        buf->cur_line++;      
        /* 
         The length of the newly created line is the number 
         of characters that were to the right of the current
         index position. 
        */
        buf->line_len[buf->cur_line] = temp - buf->line_len[buf->cur_line-1];
        buf->index_in_cur_line = 0;
    }
    else if ( isprint(c) ) {  /* non-newline character */
        buf->index_in_cur_line++;    // advance index in line
        /* increment all line starts after this line */
        for ( i = buf->cur_line+1; i < buf->num_lines; i++ )
            buf->line_start[i]++;  
    }
    else
        return UNHANDLEDCHAR;

    return 0;
}

/* Returns the input mode in case of error */
int handle_insertion( Buffer *buf, Window *win, Cursor *curs, char c)
{
    int retvalue;

    /* insert typed char and echo it */
    retvalue = insert(buf, *win,  c);
    if ( retvalue < 0 ) {
        if ( retvalue == OUT_OF_LINES )                  
            write_status_message( MAXLINES_MSSGE, *curs);
        else if ( retvalue == OUT_OF_MEM )
            write_status_message( OUT_OF_MEM_MSSGE, *curs);
        else if ( retvalue == UNHANDLEDCHAR ) {
            write_status_message( UNHANDLEDCHAR_MSSGE, *curs); 
            return retvalue;
        }
        return 0;
    }
    /* Update the cursor position */                    
    advance_cursor( curs, *win, c);

    /* Redraw the buffer contents on the screen. The window is passed
       because it might cause vertical scrolling. The position of the 
       cursor may change as well, so it is passed. The buffer itself
       is unchanged. */
    redraw_buffer( *buf, win, curs ); 
    moveto(curs->r, curs->c);
    return 1;
}


/*******************************************************************************
                               Cursor Functions
*******************************************************************************/

/**
   Given a position within a line of text and the line's number,
   compute the position in screen coordinates of this text position.
   
*/
void get_cursor_at( Buffer buf, Window win, int index, int lineno, 
                    Cursor *curs )
{
    int total_lines_before = 0;
    int rows_in_current_textline = 0;
    int i;

    /* The first line is the one at the top of the window, whose index is
       win.line_at_top, initially 0. */
    for ( i = win.line_at_top; i < lineno; i++ ) {
        if ( buf.line_len[i] < win.cols )
            total_lines_before++;
        else
            total_lines_before += (int) ceil((double)buf.line_len[i]/win.cols);
    }
    rows_in_current_textline = index/win.cols;
    
    curs->r = total_lines_before + rows_in_current_textline;
    curs->c = index - rows_in_current_textline * win.cols;
}


/******************************************************************************/
/** advance_cursor()
 *  Advances the cursor's physical position on the screen, based upon the 
 *  window's dimensions and the value of the character.
 *  @param Cursor *cursor [inout]  cursor current position on entry
 *  @param Window  win    [in]     window in which cursor appears
 *  @param char    ch     [in]     character causing movement
 */
void advance_cursor( Cursor *cursor, Window win, char ch)
{
    if ( ch == '\n' ) {
        cursor->r++;  
        cursor->c = 0;
    }
    else {
        cursor->c++;
        if ( cursor->c == win.cols ) {  /* wrap the line */
            cursor->c = 0;
            cursor->r++;
        }           
    }
}

/******************************************************************************/

void init_cursor( Cursor *cursor )
{
    cursor->r         = 0;
    cursor->c         = 0;
}

/*****************************************************************************/
/*
    The ANSI escapes to save and restore are not honored by
    all terminal emulators, so this function uses the moveto() function.
*/

void show_cursor( Buffer buf, Window win, Cursor cursor, 
                  int index_in_line, int line_number )
{
    char curs_str[80];

    sprintf(curs_str, "Cursor: [%d,%d]  line index: %d  win topline: %d "
                      "buf #lines: %d",
                cursor.r+1, cursor.c+1, line_number, win.line_at_top,
                buf.num_lines  ) ;
    write(1,PARK, lPARK);
    write(1, CLEAR_LINE, lCLEAR_LINE);
    write(1, curs_str, strlen(curs_str));
    moveto(cursor.r, cursor.c);
}

/******************************************************************************/

void moveUp( Buffer *buf, Window *win, Cursor *curs   )
{
    /* if buf.cur_line == 0, we cannot go up further */
    if ( buf->cur_line > 0 ) {
        buf->cur_line--;
        /* Check whether the cursor would be past the rightmost character 
           of the now current line. If so, position it just past the rightmost 
           character. */
        if ( buf->index_in_cur_line >= buf->line_len[buf->cur_line] ) {
            buf->index_in_cur_line = buf->line_len[buf->cur_line]-1;
        }
        /* Check if the cursor would be above the uppermost visible line on 
           the screen. If it isn't no scrolling is necessary, but if it is, 
           we have to recalculate the new top line and scroll. */
        if ( buf->cur_line >= win->line_at_top ) {
            /* No scrolling necessary; the cursor is within a visible line */
            get_cursor_at(*buf, *win, buf->index_in_cur_line, buf->cur_line, 
                              curs ); 
            }
        else {
            /* Need to scroll */
            win->line_at_top = buf->cur_line;
            get_cursor_at(*buf, *win, buf->index_in_cur_line, buf->cur_line, 
                              curs ); 
            scroll_buffer(*buf, *win ); 
        }                               
       moveto(curs->r, curs->c);
    }
}

/******************************************************************************/

void moveDown( Buffer *buf, Window *win, Cursor *curs )
{
    int lastline;

    if ( buf->cur_line < buf->num_lines-1 ) {
        buf->cur_line++;
        /* Check whether the cursor would be past the rightmost character 
           of the now current line. If so, position it just past the rightmost 
           character. */
        if ( buf->index_in_cur_line >= buf->line_len[buf->cur_line] ) {
            buf->index_in_cur_line = buf->line_len[buf->cur_line]-1;
        }

        get_lastline_in_win( *buf, *win,  &lastline );

        if ( buf->cur_line > lastline ) {
            /* Need to scroll */
            win->line_at_top += buf->cur_line - lastline;
            get_cursor_at(*buf, *win, buf->index_in_cur_line, buf->cur_line, 
                              curs ); 
            scroll_buffer(*buf, *win ); 
        }
        else 
            get_cursor_at(*buf, *win, buf->index_in_cur_line,  
                           buf->cur_line,  curs ); 

        moveto(curs->r, curs->c);
    }
}

/******************************************************************************/

void moveRight( Buffer *buf, Window *win, Cursor *curs )
{
    if ( ( buf->index_in_cur_line < buf->line_len[buf->cur_line] -1 ) ||
       ( ( buf->index_in_cur_line < buf->line_len[buf->cur_line] )
          && ( buf->cur_line == buf->num_lines-1 ) ) )

    { 
        buf->index_in_cur_line++; 
        if ( buf->index_in_cur_line % win->cols == 0 ) {
            curs->r++;
            curs->c = 0;
            if ( curs->r > win->rows-2 ) {
               win->line_at_top += curs->r - (win->rows-2);
               scroll_buffer(*buf, *win ); 
            }
            moveto(curs->r, curs->c);
        }
        else {
            curs->c++;
            write(1, RIGHT, lRIGHT);
        }
    }
}

/******************************************************************************/

void moveLeft ( Buffer *buf, Window *win, Cursor *curs )
{
    if ( buf->index_in_cur_line > 0 ) {
        if ( buf->index_in_cur_line % win->cols == 0 ) {
            curs->r--;
            curs->c = win->cols-1;
            moveto(curs->r, curs->c);
        }
        else {
            curs->c--;
            write(1, LEFT, lLEFT);
        }
        buf->index_in_cur_line--; 
    }
}

