/* $Id$
 * $URL$
 *
 * Copyright (C) 2009 Scott Sibley <scott@starlon.net>
 *
 * This file is part of LCDControl.
 *
 * LCDControl is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LCDControl is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LCDControl.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __generator_h__
#define __generator_h__

// generator/continuation for C++
// author: Andrew Fedoniouk @ terrainformatica.com
// idea borrowed from: "coroutines in C" Simon Tatham,
// http://www.chiark.greenend.org.uk/~sgtatham/coroutines.html

struct _generator
{
  int _line;
  _generator():_line(0) {}
};


#define test2 2

#define _generator(NAME) struct NAME : public _generator

#define _emit(T) bool operator()(T &_rv1, T &_rv2) { \
                    switch(_line) { case 0:;

#define _stop  } _line = 0; return false; }

#define _yield(V)     \
        do {\
            _line=__LINE__;\
            _rv2 = _rv1;\
            _rv1 = (V); return true; case __LINE__:;\
        } while (0)
#endif

