/* =========================================================================
 * This file is part of str-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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


#pragma once
#ifndef CODA_OSS_str_Format_h_INCLUDED_
#define CODA_OSS_str_Format_h_INCLUDED_

#include <stdarg.h>
#include<stdint.h>
#include<stddef.h>

#include <string>

#include "config/Exports.h"

namespace str
{
struct CODA_OSS_API Format final
{
    Format(const char* format, ...);

    operator std::string() const
    {
        return mString;
    }

private:
    std::string mString;
};

}

/*!
 *  \param format  The format
 *  \param ... Any printf like thing
 */
//CODA_OSS_API std::string format(const char* format, ...);

inline std::string FmtX(const char* format)
{
    return str::Format(format);
}

inline std::string FmtX(const char* format, const char* pStr)
{
    return str::Format(format, pStr);
}
inline std::string FmtX(const char* format, const std::string& s)
{
    return str::Format(format, s.c_str());
}
inline std::string FmtX(const char* format, int i)
{
    return str::Format(format, i);
}
inline std::string FmtX(const char* format, uint32_t i)
{
    return str::Format(format, i);
}
inline std::string FmtX(const char* format, ptrdiff_t l)
{
    return str::Format(format, l);
}
inline std::string FmtX(const char* format, size_t ul)
{
    return str::Format(format, ul);
}
inline std::string FmtX(const char* format, float f)
{
    return str::Format(format, f);
}

inline std::string FmtX(const char* format, char ch, const char* pStr)
{
    return str::Format(format, ch, pStr);
}
inline std::string FmtX(const char* format, char ch, const std::string& s)
{
    return str::Format(format, ch, s.c_str());
}
inline std::string FmtX(const char* format, const std::string& s1, const std::string& s2)
{
    return str::Format(format, s1.c_str(), s2.c_str());
}
inline std::string FmtX(const char* format, const std::string& s, size_t ul)
{
    return str::Format(format, s.c_str(), ul);
}
inline std::string FmtX(const char* format, char ch1, char ch2)
{
    return str::Format(format, ch1, ch2);
}
inline std::string FmtX(const char* format, size_t ul1, size_t ul2)
{
    return str::Format(format, ul1, ul2);
}
inline std::string FmtX(const char* format, int i, const std::string& s)
{
    return str::Format(format, i, s.c_str());
}
inline std::string FmtX(const char* format, int i, const char* pStr)
{
    return str::Format(format, i, pStr);
}

inline std::string FmtX(const char* format, char ch1, char ch2, const std::string& s)
{
    return str::Format(format, ch1, ch2, s.c_str());
}
inline std::string FmtX(const char* format, int i1, int i2, unsigned long ul)
{
    return str::Format(format, i1, i2, ul);
}
inline std::string FmtX(const char* format, int i1, int i2, int i3)
{
    return str::Format(format, i1, i2, i3);
}
inline std::string FmtX(const char* format, const std::string& s1, const std::string& s2, uint32_t ui)
{
    return str::Format(format, s1.c_str(), s2.c_str(), ui);
}

#endif  // CODA_OSS_str_Format_h_INCLUDED_