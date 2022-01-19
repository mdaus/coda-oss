/* =========================================================================
 * This file is part of str-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * (C) Copyright 2022, Maxar Technologies, Inc.
 *
 * str-c++ is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public 
 * License along with this program; If not, 
 * see <http://www.gnu.org/licenses/>.
 *
 */

#include "str/EncodedString.h"

str::EncodedString::EncodedString(const std::string& s) : s_(s), v_(s_) { }

str::EncodedString::EncodedString(const sys::U8string& s) :
    s_(c_str<std::string::const_pointer>(s)), v_(EncodedStringView::fromUtf8(s_)) { }

str::EncodedString::EncodedString(const str::W1252string& s) :
    s_(c_str<std::string::const_pointer>(s)), v_(EncodedStringView::fromWindows1252(s_)) { }

str::EncodedString::EncodedString(const std::u16string& s) : EncodedString(to_u8string(s)) { }
str::EncodedString::EncodedString(const std::u32string& s) : EncodedString(to_u8string(s)) { }

// create from a view
str::EncodedString& str::EncodedString::operator=(const EncodedStringView& v)
{
    EncodedStringView::details::assign(v, *this);
    return *this;
}
str::EncodedString::EncodedString(const EncodedStringView& v)
{
    *this = v;
}

str::EncodedString str::EncodedString::fromUtf8(const std::string& s)
{
    return str::EncodedStringView::fromUtf8(s);
}
str::EncodedString str::EncodedString::fromWindows1252(const std::string& s)
{
    return str::EncodedStringView::fromWindows1252(s);
}