/* =========================================================================
 * This file is part of io-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * io-c++ is free software; you can redistribute it and/or modify
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

#include <string>

#include <sys/Conf.h>
#include <except/Exception.h>
#include <io/OutputStream.h>

// Convert a single ISO8859-1 character to UTF-8
// // https://en.wikipedia.org/wiki/ISO/IEC_8859-1
static std::string toUtf8_(std::string::value_type ch)
{
    std::string retval;

    if ((ch >= '\x00') && (ch <= '\x7f'))  // ASCII
    {
        retval.push_back(ch);
    }
    else if ((ch >= '\xC0' /*À*/) && (ch <= '\xFF' /*y*/))  // ISO8859-1 letters
    {
        retval.push_back('\xC3');
        ch -= 0x40;  // 0xC0 -> 0x80
        retval.push_back(ch);
    }
    else
    {
        retval.push_back(ch);  // ???
    }

    return retval;
}

static std::string toUtf8(const std::string& str)
{
    std::string retval;
    // Assume the input string is ISO8859-1 (western European) and convert to UTF-8
    for (const auto& ch : str)
    {
        retval += toUtf8_(ch);
    }
    return retval;
}

void io::OutputStream::write(const std::string& str_, bool asUTF8 /*= false*/)
{
    const std::string* pStr = &str_;
    std::string str;
    if (asUTF8)
    {
        str = toUtf8(str_);
        pStr = &str;
    }
     
    auto buffer = reinterpret_cast<const sys::byte*>(pStr->c_str());
    const sys::Size_T len{pStr->length()};
    write(buffer, len);
}
