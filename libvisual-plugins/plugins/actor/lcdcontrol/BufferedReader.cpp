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
#include <cstring>
#include <sstream>
#include "BufferedReader.h"
#include "debug.h"

using namespace LCD;

BufferedReader::BufferedReader() {
    current_ = 0;
    locked_ = false;
}

int BufferedReader::Peek(unsigned char *data, int bytes) {
    int i;
    for(i = current_; (i - current_) < bytes && 
        i < (int)buffer_.size() ; i++ ) {
        data[i-current_] = buffer_[i];
    }
    int read = i - current_;
    current_+=i - current_;
    return read;
}

int BufferedReader::Read(unsigned char *data, int bytes) {
    int i;
    std::vector<unsigned char>::iterator it = buffer_.begin();
    for( i = 0; i < bytes && i < (int)buffer_.size(); i++ ) {
        data[i] = buffer_[i];
        it++;
    }
    buffer_.erase(buffer_.begin(), it);
    current_ = 0;
    return i;
}

void BufferedReader::AddData(unsigned char *data, int len) {
    for( int i = 0; i < len; i++ ) {
        buffer_.push_back(data[i]);
    }
}

void BufferedReader::SetCurrent(int c) {
    current_ = c;
}

int BufferedReader::GetCurrent() {
    return current_;
}

void BufferedReader::SetLocked(bool locked) {
    locked_ = locked;
}

bool BufferedReader::GetLocked() {
    return locked_;
}
