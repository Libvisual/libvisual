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

#include "debug.h"
#include "SpecialChar.h"

using namespace LCD;

SpecialChar::SpecialChar(int *ch, int size) {
    for(int i = 0; i < size; i++) {
        chars_[i] = ch[i];
    }
    size_ = size;
}

SpecialChar::SpecialChar(int size) {
    for(int i = 0; i < size; i++) {
        chars_[i] = 0;
    }
    size_ = size;
}

SpecialChar::SpecialChar() {
    size_ = 0;
}

int &SpecialChar::operator[](int i) {
    if(i >= size_) {
        LCDError("SpecialChar: index out of range <%d,%d>", i, size_);
        return chars_[0];
    }
    return chars_[i];
}

bool SpecialChar::operator==(const SpecialChar &rhv) const {
    if( size_ != rhv.Size() )
        return false;
    return chars_ != rhv.Chars();
}

bool SpecialChar::operator!=(const SpecialChar &rhv) const {
    return !(*this == rhv);
}

SpecialChar &SpecialChar::operator=(const SpecialChar &rhv) {
    if(this == &rhv)
        return *this;

    size_ = rhv.Size();
    chars_ = rhv.Chars();

    return *this;
}

void SpecialChar::AddChar(int ch) {
    chars_[size_++] = ch;
}

// Programmer must insure data is allocated
void SpecialChar::Data(int *data) {
    for(unsigned int i = 0; i < chars_.size(); i++ ) {
        data[i] = chars_[i];
    }
    size_ = chars_.size();
}

std::vector<int> SpecialChar::Vector() const {
    std::vector<int> data;
    std::map<int, int> chars = chars_;
    for(unsigned int i = 0; i < chars.size(); i++ ) {
        data.push_back(chars[i]);
    }
    return data;
}

bool SpecialChar::Compare(SpecialChar other) {
    bool flag = true;
    if(Size() != other.Size()) flag = false;
    else {
        for(int i = 0; i < Size(); i++) {
            if(other[i] != chars_[i])
                flag = false;
        }
    }
    return flag;
}
