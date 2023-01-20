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

#ifndef __SPECIAL_CHAR_H__
#define __SPECIAL_CHAR_H__

#include <map>
#include <vector>

namespace LCD {

class SpecialChar {
    std::map<int, int> chars_;
    int size_;

    public:
    SpecialChar();
    SpecialChar(int *ch, int size);
    SpecialChar(int size);
    int Size() const { return size_; };
    std::map<int, int> Chars() const { return chars_; };
    void AddChar(int ch);
    void Data(int *data);
    std::vector<int> Vector() const;
    bool Compare(SpecialChar other);
    int &operator[](int i);
    bool operator==(const SpecialChar &ch) const;
    bool operator!=(const SpecialChar &ch) const;
    SpecialChar &operator=(const SpecialChar &rhv);
};

}; // End namespace

#endif
