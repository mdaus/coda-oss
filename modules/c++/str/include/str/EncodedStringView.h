/* =========================================================================
 * This file is part of str-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * (C) Copyright 2022, Maxar Technologies, Inc.
 *
 * xml.lite-c++ is free software; you can redistribute it and/or modify
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

#ifndef CODA_OSS_str_EncodedStringView_h_INCLLUDED_
#define CODA_OSS_str_EncodedStringView_h_INCLLUDED_
#pragma once

#include <string>

#include "str/Encoding.h"

/*!
 * \file EncodedStringView.h
 * \brief A String that can be either UTF-8 or "native" 
 *
 * On Linux, there is good support for UTF-8, so a std::string encoded
 * as UTF-8 will display the "foreign" characters properly.  On Windows,
 * the preferred way to do that is by using UTF-16 (WCHAR, std::wstring),
 * but little (none?) of our existing code bases do that.  (On Linux, std::wstring
 * is typically UTF-32.)
 *
 */

namespace str
{
class EncodedStringView final
{
    const std::string* mpString = nullptr;
    const sys::U8string* mpU8String = nullptr;
    const str::W1252string* mpW1252String = nullptr;

public:
    EncodedStringView() = default;
    EncodedStringView(const sys::U8string&);
    EncodedStringView(const str::W1252string&);
    // Assume platform native encoding: UTF-8 on Linux, Windows-1252 on Windows
    explicit EncodedStringView(const std::string&);  

    ~EncodedStringView() = default;
    EncodedStringView(const EncodedStringView&) = default;
    EncodedStringView& operator=(const EncodedStringView&) = default;
    EncodedStringView(EncodedStringView&&) = default;
    EncodedStringView& operator=(EncodedStringView&&) = default;

    size_t size() const;
    size_t length() const
    {
        return size();
    }

    template<typename T>
    const T& cref() const;  // no conversion, might throw
    template <typename T>
    const T& ref() const
    {
        return cref<T>();
    }

    const char* c_str() const; // either mpString->c_str() or mpU8String->c_str()

    // Regardless of what string we're looking at, return a string in platform
    // native encoding: UTF-8 on Linux, Windows-1252 on Windows; this
    // might result in string conversion.
    std::string native() const; // c.f. std::filesystem::path::native()
};

// GCC wants the specializations outside of the class
template <>
inline const std::string& EncodedStringView::cref() const  // no conversion, might throw
{
    return *mpString;
}
template <>
inline const sys::U8string& EncodedStringView::cref() const  // no conversion, might throw
{
    return *mpU8String;
}
template <>
inline const str::W1252string& EncodedStringView::cref() const  // no conversion, might throw
{
    return *mpW1252String;
}

}
#endif  // CODA_OSS_str_EncodedStringView_h_INCLLUDED_
