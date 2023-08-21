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
#include <ostream>

#include "coda_oss/span.h"
#include "config/Exports.h"
 #include "str/Encoding.h"

/*!
 * \file EncodedStringView.h
 * \brief A read-only "view" onto a string.
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
class EncodedString; // forward
class CODA_OSS_API EncodedStringView final
{
    enum class Encoding
    {
        Utf8, // native on Linux
        Windows1252, // native for std::string on Windows
        Utf16, // std::u16string, native on Windows (Win32 API)
        Utf32, // std::u32string, native on Linux (not widely used)
    };

    const void* data_ = nullptr;
    size_t size_ = 0; // strlen()
    coda_oss::span<const char> mString;

    explicit EncodedStringView(coda_oss::span<const coda_oss::u8string::value_type>);
    explicit EncodedStringView(coda_oss::span<const std::u16string::value_type>);
    explicit EncodedStringView(coda_oss::span<const std::u32string::value_type>);
    explicit EncodedStringView(coda_oss::span<const std::string::value_type>);
    explicit EncodedStringView(coda_oss::span<const str::W1252string::value_type>);

    #ifdef _WIN32
    static constexpr Encoding mStringNative = Encoding::Windows1252;
    #else
    static constexpr Encoding mStringNative = Encoding::Utf8;  // !_WIN32, assume Linux
    #endif
    Encoding mEncoding = mStringNative;
    bool mIsUtf8 = true;

    // Want to create an EncodedString from EncodedStringView.  The public interface
    // doesn't expose "mIsUtf8" so there's (intentinally) no way for clients to know the encoding.
    friend EncodedString;

    str::W1252string w1252string() const;  // c.f. std::filesystem::path::u8string()

public:
    EncodedStringView() = default;
    ~EncodedStringView() = default;
    EncodedStringView(const EncodedStringView&) = default;
    EncodedStringView& operator=(const EncodedStringView&) = default;
    EncodedStringView(EncodedStringView&&) = default;
    EncodedStringView& operator=(EncodedStringView&&) = default;

    // Need the const char* overloads to avoid creating temporary std::basic_string<> instances.
    // Routnes always return a copy, never a reference, so there's no additional overhead
    // with storing a raw pointer rather than a pointer to  std::basic_string<>.
    EncodedStringView(coda_oss::u8string::const_pointer, coda_oss::u8string::size_type);
    explicit EncodedStringView(coda_oss::u8string::const_pointer);
    explicit EncodedStringView(const coda_oss::u8string&);

    EncodedStringView(std::u16string::const_pointer, std::u16string::size_type);
    explicit EncodedStringView(std::u16string::const_pointer);
    explicit EncodedStringView(const std::u16string&); 

    EncodedStringView(std::u32string::const_pointer, std::u32string::size_type);
    explicit EncodedStringView(std::u32string::const_pointer);
    explicit EncodedStringView(const std::u32string&); 

    EncodedStringView(str::W1252string::const_pointer, str::W1252string::size_type);
    explicit EncodedStringView(str::W1252string::const_pointer);
    explicit EncodedStringView(const str::W1252string&);

    EncodedStringView(std::string::const_pointer, std::string::size_type);
    explicit EncodedStringView(std::string::const_pointer);  // Assume platform native encoding: UTF-8 on Linux, Windows-1252 on Windows
    explicit EncodedStringView(const std::string&);  // Assume platform native encoding: UTF-8 on Linux, Windows-1252 on Windows

    // Regardless of what string we're looking at, return a string in platform
    // native encoding: UTF-8 on Linux, Windows-1252 on Windows; this
    // might result in string conversion.
    std::string native() const; // c.f. std::filesystem::path::native()

    // Convert (perhaps) whatever we're looking at to UTF-8
    coda_oss::u8string u8string() const;  // c.f. std::filesystem::path::u8string()

    // Convert whatever we're looking at to UTF-16 or UTF-32
    std::u16string u16string() const;  // c.f. std::filesystem::path::u8string()
    std::u32string u32string() const;  // c.f. std::filesystem::path::u8string()
    // This is especially useful on Windows because the default for characters
    // is WCHAR (char* is converted to UTF-16).
    std::wstring wstring() const; // UTF-16 on Windows, UTF-32 on Linux

    // These are for "advanced" use, most "normal" code should use the routines above.
    std::string::const_pointer c_str() const noexcept
    {
        return (mEncoding == Encoding::Utf8) ||  (mEncoding == Encoding::Windows1252) ?
            static_cast<std::string::const_pointer>(data_) : nullptr;
    }
    coda_oss::u8string::const_pointer c_u8str() const
    {
        return (mEncoding == Encoding::Utf8) ? static_cast<coda_oss::u8string::const_pointer>(data_) : nullptr;
    }
    std::u16string::const_pointer c_u16str() const
    {
        return (mEncoding == Encoding::Utf16) ? static_cast<std::u16string::const_pointer>(data_) : nullptr;
    }
    std::u32string::const_pointer c_u32str() const
    {
        return (mEncoding == Encoding::Utf32) ? static_cast<std::u32string::const_pointer>(data_) : nullptr;
    }
    str::W1252string::const_pointer c_w1252str() const
    {
        return (mEncoding == Encoding::Windows1252) ? static_cast<str::W1252string::const_pointer>(data_) : nullptr;
    }
    size_t size() const noexcept
    {
        return size_;
    }

    // Input is encoded as specified on all platforms.
    static EncodedStringView fromUtf8(const std::string& utf8)
    {
        return EncodedStringView(str::c_str<coda_oss::u8string>(utf8),  utf8.length());
    }
    static EncodedStringView fromUtf8(std::string::const_pointer pUtf8)
    {
        return EncodedStringView(str::cast<coda_oss::u8string::const_pointer>(pUtf8));
    }
    static EncodedStringView fromWindows1252(const std::string& w1252)
    {
        return EncodedStringView(str::c_str<str::W1252string>(w1252), w1252.length());
    }
    static EncodedStringView fromWindows1252(std::string::const_pointer pW1252)
    {
        return EncodedStringView(str::cast<str::W1252string::const_pointer>(pW1252));
    }

    std::string asUtf8() const;
    std::string asWindows1252() const;

    bool operator_eq(const EncodedStringView&) const;

    struct details final
    {
        // Convert (perhaps) whatever we're looking at to Windows-1252
        // Intended for unit-testing; normal use is native().
        static str::W1252string w1252string(const EncodedStringView& v)
        {
            return v.w1252string();
        }
    };
};

inline bool operator==(const EncodedStringView& lhs, const EncodedStringView& rhs)
{
    return lhs.operator_eq(rhs);
}
inline bool operator!=(const EncodedStringView& lhs, const EncodedStringView& rhs)
{
    return !(lhs == rhs);
}

// Since we'd really like to "traffic" in UTF-8 strings (at least when encoding is a consideration)
// make that comparision easy.
inline bool operator==(const EncodedStringView& lhs, const coda_oss::u8string& rhs)
{
    return lhs == EncodedStringView(rhs);
}
inline bool operator!=(const EncodedStringView& lhs, const coda_oss::u8string& rhs)
{
    return !(lhs == rhs);
}
inline bool operator==(const coda_oss::u8string& lhs, const EncodedStringView& rhs)
{
    return rhs == lhs;
}
inline bool operator!=(const coda_oss::u8string& lhs, const EncodedStringView& rhs)
{
    return !(lhs == rhs);
}

inline std::string toString(const EncodedStringView& esv) 
{
    return esv.native();
}

inline std::ostream& operator<<(std::ostream& os, const EncodedStringView& esv)
{
    os << toString(esv);
    return os;
}

}
#endif  // CODA_OSS_str_EncodedStringView_h_INCLLUDED_
