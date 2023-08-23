/* =========================================================================
 * This file is part of str-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * (C) Copyright 2020, 2021, 2022, Maxar Technologies, Inc.
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

#ifndef CODA_OSS_str_Encoding_h_INCLUDED_
#define CODA_OSS_str_Encoding_h_INCLUDED_
#pragma once

#include <string.h>
#include <wchar.h>
#include <stdint.h>

#include <memory>
#include <string>
#include <type_traits>

#include "coda_oss/string.h"
#include "gsl/gsl.h"
#include "config/Exports.h"

namespace str
{
template <typename TReturn, typename TChar>
inline TReturn cast(const TChar* s)
{
    // This is OK as UTF-8 can be stored in std::string
    // Note that casting between the string types will CRASH on some
    // implementations. NO: reinterpret_cast<const std::string&>(value)
    const void* const pStr = s;
    auto const retval = static_cast<TReturn>(pStr);
    static_assert(sizeof(*retval) == sizeof(*s), "sizeof(*TReturn) != sizeof(*TChar)"); 
    return retval;
}
template <typename TBasicStringT, typename TChar>
inline typename TBasicStringT::const_pointer c_str(const std::basic_string<TChar>& s)
{
    using return_t = typename TBasicStringT::const_pointer;
    return cast<return_t>(s.c_str());
}

// This is to make it difficult to get encodings mixed up; it's here (in a .h
// file) as we want to unit-test it. Windows1252_T for Windows-1252 characters
enum class Windows1252_T : unsigned char { };  // https://en.cppreference.com/w/cpp/language/types
using W1252string = std::basic_string<Windows1252_T>;  // https://en.cppreference.com/w/cpp/string

//////////////////////////////////////////////////////////////////////////////////////////

// We'll get strange errors, possibibly at link-time, if wchar_t is not a wchar_t type.
// MSVC has an option to control this: https://docs.microsoft.com/en-us/cpp/build/reference/zc-wchar-t-wchar-t-is-native-type
// https://en.cppreference.com/w/cpp/language/types
// "It has the same size, signedness, and alignment as one of the integer types, but is a distinct type."
static_assert(!std::is_same<wchar_t, uint16_t>::value, "wchar_t should not be the same as uint16_t");
static_assert(!std::is_same<wchar_t, int16_t>::value, "wchar_t should not be the same as int16_t");
static_assert(!std::is_same<wchar_t, uint32_t>::value, "wchar_t should not be the same as uint32_t");
static_assert(!std::is_same<wchar_t, int32_t>::value, "wchar_t should not be the same as int32_t");

// When the encoding is important, we want to "traffic" in coda_oss::u8string (UTF-8), not
// str::W1252string (Windows-1252) or std::string (unknown).  Make it easy to get those from other encodings.
CODA_OSS_API coda_oss::u8string to_u8string(str::W1252string::const_pointer, size_t);
inline coda_oss::u8string to_u8string(coda_oss::u8string::const_pointer s, size_t sz)
{
    return coda_oss::u8string(s, sz);
}
CODA_OSS_API coda_oss::u8string to_u8string(std::string::const_pointer, size_t); // platform determines Windows-1252 or UTF-8 input
CODA_OSS_API coda_oss::u8string to_u8string(std::wstring::const_pointer, size_t); // platform determines UTF16 or UTF-32 input

// UTF-16 is typically uses on Windows (where it is std::wstring::value_type); Linux prefers UTF-32.
CODA_OSS_API coda_oss::u8string to_u8string(std::u16string::const_pointer, size_t);

CODA_OSS_API std::u16string to_u16string(coda_oss::u8string::const_pointer, size_t);
inline auto to_u16string(const coda_oss::u8string& s)
{
    return to_u16string(s.c_str(), s.length());
}
CODA_OSS_API std::u16string to_u16string(str::W1252string::const_pointer, size_t);
inline auto to_u16string(const str::W1252string& s)
{
    return to_u16string(s.c_str(), s.length());
}

CODA_OSS_API std::u16string to_u16string(const std::string&); // platform determines Windows-1252 or UTF-8 input
CODA_OSS_API std::u16string to_u16string(const std::wstring&); // platform determines UTF16 or UTF-32 input

// UTF-32 is convenient because each code-point is a single 32-bit integer.
// It's typically std::wstring::value_type on Linux, but NOT Windows.
CODA_OSS_API coda_oss::u8string to_u8string(std::u32string::const_pointer, size_t);
CODA_OSS_API std::u32string to_u32string(coda_oss::u8string::const_pointer, size_t);
inline auto to_u32string(const coda_oss::u8string& s)
{
    return to_u32string(s.c_str(), s.length());
}
CODA_OSS_API std::u32string to_u32string(str::W1252string::const_pointer, size_t);
inline auto to_u32string(const str::W1252string& s)
{
    return to_u32string(s.c_str(), s.length());
}
CODA_OSS_API std::u32string to_u32string(const std::wstring&); // platform determines UTF16 or UTF-32 input

template <typename TChar>
inline coda_oss::u8string to_u8string(const std::basic_string<TChar>& s)
{
    return to_u8string(s.c_str(), s.size());
}
template <typename TChar>
inline coda_oss::u8string to_u8string(const TChar* p)
{
    return to_u8string(std::basic_string<TChar>(p));
}

CODA_OSS_API str::W1252string to_w1252string(coda_oss::u8string::const_pointer p, size_t sz);
inline str::W1252string to_w1252string(const coda_oss::u8string& s)
{
    return to_w1252string(s.c_str(), s.length());
}
CODA_OSS_API str::W1252string to_w1252string(const std::wstring&);
CODA_OSS_API str::W1252string to_w1252string(const std::u16string&);

CODA_OSS_API std::string to_string(const coda_oss::u8string&); // platform determines Windows-1252 or UTF-8 output
inline std::string to_string(const std::string& s)
{
    return s;
}
CODA_OSS_API std::string to_string(const std::wstring&);
CODA_OSS_API std::wstring to_wstring(const std::string&); // platform determines Windows-1252 or UTF-8 input and output encoding
CODA_OSS_API std::wstring to_wstring(const coda_oss::u8string&); // platform determines UTF-16 or UTF-32 output encoding
CODA_OSS_API std::wstring to_wstring(const std::u16string&); // platform determines UTF-16 or UTF-32 output encoding
CODA_OSS_API std::wstring to_wstring(const str::W1252string&);
CODA_OSS_API std::string to_string(const std::u16string&);
CODA_OSS_API std::string to_string(const str::W1252string&);

CODA_OSS_API coda_oss::u8string from_utf8(const std::string&); // input encoding is *always* UTF-8
CODA_OSS_API std::string as_utf8(const coda_oss::u8string&); // output encoding is *always* UTF-8

CODA_OSS_API coda_oss::u8string from_windows1252(const std::string&); // input encoding is *always* Windows-1252
CODA_OSS_API std::string as_windows1252(const coda_oss::u8string&); // output encoding is *always* Windows-1252

namespace details // YOU should use EncodedStringView
{
void w1252to8(str::W1252string::const_pointer p, size_t sz, std::string&); // encoding is lost
void utf8to1252(coda_oss::u8string::const_pointer p, size_t sz, std::string&); // encoding is lost
}
}

#endif // CODA_OSS_str_Encoding_h_INCLUDED_
