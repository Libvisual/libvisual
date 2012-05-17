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

#include <vector>
#include <string>

#ifndef __BUFFERED_READER__
#define __BUFFERED_READER__

namespace LCD {

class BufferedReader {
    std::vector<unsigned char> buffer_;
    bool locked_;
    int current_;
    public:
    BufferedReader();
    int Peek(unsigned char *data, int bytes = 1);
    int Read(unsigned char *data, int bytes = 1);
    void AddData(unsigned char *data, int len);
    void SetCurrent(int c);
    int GetCurrent();
    void SetLocked(bool locked);
    bool GetLocked();
};

}

#endif
