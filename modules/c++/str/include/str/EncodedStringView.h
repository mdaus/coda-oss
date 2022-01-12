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

#include <assert.h>

#include <string>
#include <ostream>

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
    std::string::const_pointer mpString = nullptr;
    sys::U8string::const_pointer mpU8String = nullptr;
    str::W1252string::const_pointer mpW1252String = nullptr;

public:
    EncodedStringView() = delete; // use EncodedStringView("") if you must

    // Need these overload to avoid creating temporary std::basic_string<> instances.
    // Routnes always return a copy, never a reference, so there's no additional overhead
    // with storing a raw pointer rather than a pointer to  std::basic_string<>.
    EncodedStringView(sys::U8string::const_pointer);
    EncodedStringView(str::W1252string::const_pointer);
    explicit EncodedStringView(std::string::const_pointer);  // Assume platform native encoding: UTF-8 on Linux, Windows-1252 on Windows
    template<typename TChar>
    EncodedStringView(const std::basic_string<TChar>& s) : EncodedStringView(s.c_str())
    {
    }

    EncodedStringView& operator=(sys::U8string::const_pointer);
    EncodedStringView& operator=(str::W1252string::const_pointer);
    EncodedStringView& operator=(std::string::const_pointer);  // Assume platform native encoding: UTF-8 on Linux, Windows-1252 on Windows
    template <typename TChar>
    EncodedStringView& operator=(const std::basic_string<TChar>& s)
    {
        *this = s.c_str();
        return *this;
    }
    
    // Input is encoded as specified on all platforms.
    template <typename TBasicString>
    EncodedStringView& assign(const char* s)
    {
        using const_pointer = typename TBasicString::const_pointer;
        *this = str::cast<const_pointer>(s);
        return *this;
    }
    template <typename TBasicString>
    EncodedStringView& assign(const std::string& s)
    {
        return assign<TBasicString>(s.c_str());
    }

    ~EncodedStringView() = default;
    EncodedStringView(const EncodedStringView&) = default;
    EncodedStringView& operator=(const EncodedStringView&) = default;
    EncodedStringView(EncodedStringView&&) = default;
    EncodedStringView& operator=(EncodedStringView&&) = default;

    // Input is encoded as specified on all platforms.
    template <typename TBasicString>
    static EncodedStringView create(const char* s)
    {
        using const_pointer = typename TBasicString::const_pointer;
        return EncodedStringView(str::cast<const_pointer>(s));
    }
    template <typename TBasicString>
    static EncodedStringView create(const std::string& s)
    {
        return create<TBasicString>(s.c_str());
    }

    // Regardless of what string we're looking at, return a string in platform
    // native encoding: UTF-8 on Linux, Windows-1252 on Windows; this
    // might result in string conversion.
    std::string native() const; // c.f. std::filesystem::path::native()

    // Convert (perhaps) whatever we're looking at to UTF-8
    sys::U8string to_u8string() const;
    std::string& toUtf8(std::string&) const; // std::string is encoded as UTF-8, always.

    // Only casting done, no conversion.  This should be OK as all three
    // string types are 8-bit encodings.
    //
    // Intentionally a bit of a mouth-full as these routines should be used sparingly.
    template <typename TConstPointer>
    TConstPointer cast() const
    {
        return static_cast<TConstPointer>(data_());
    }
    template <typename TConstPointer>
    TConstPointer try_cast() const;  // returns NULL if stored pointer not of the desired type

    bool operator_eq(const EncodedStringView&) const;

private:
    const void* data_() const;
    template <typename TReturn, typename T2, typename T3>
    TReturn try_cast_(TReturn retval, T2 t2, T3 t3) const
    {
        if (retval != nullptr)
        {
            assert(t2 == nullptr);
            assert(t3 == nullptr);
            return retval;
        }
        return nullptr;
    }
};

// GCC wants specializations outside of the class
template <>
inline std::string::const_pointer EncodedStringView::try_cast() const
{
    return try_cast_(mpString, mpU8String, mpW1252String);
}
template <>
inline sys::U8string::const_pointer EncodedStringView::try_cast() const
{
    return try_cast_(mpU8String, mpString, mpW1252String);
}
template <>
inline str::W1252string::const_pointer EncodedStringView::try_cast() const
{
    return try_cast_(mpW1252String, mpString, mpU8String);
}

inline bool operator==(const EncodedStringView& lhs, const EncodedStringView& rhs)
{
    return lhs.operator_eq(rhs);
}
inline bool operator!=(const EncodedStringView& lhs, const EncodedStringView& rhs)
{
    return !(lhs == rhs);
}

template<typename TChar>
inline bool operator==(const EncodedStringView& lhs, const TChar* rhs)
{
    return lhs == EncodedStringView(rhs);
}
template <typename TChar>
inline bool operator!=(const EncodedStringView& lhs, const TChar* rhs)
{
    return !(lhs == rhs);
}

template <typename TChar>
inline bool operator==(const EncodedStringView& lhs, const std::basic_string<TChar>& rhs)
{
    return lhs == EncodedStringView(rhs);
}
template <typename TChar>
inline bool operator!=(const EncodedStringView& lhs, const std::basic_string<TChar>& rhs)
{
    return !(lhs == rhs);
}

inline std::ostream& operator<<(std::ostream& os, const EncodedStringView& esv)
{
    os << esv.native();
    return os;
}

}
#endif  // CODA_OSS_str_EncodedStringView_h_INCLLUDED_
