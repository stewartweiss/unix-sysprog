/******************************************************************************
  Title          : simple_C++_io.cpp
  Author         : Stewart Weiss
  Created on     : January 26, 2006
  Description    : Copies standard input to output a char at a time 
  Purpose        : To demonstrate typical text I/O in C++
  Usage          : simple_C++_io 
  Build with     : g++ -o simple_C++_io simple_C++_io.cpp
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
#include <iostream>
using namespace std;

/* copy from stdin to stdout using C++ */              
int main()
{
	char c;
	while ( (c = cin.get()) && !cin.eof() )
		cout.put(c);
	return 0;
}
