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

#include "str/EncodedStringView.h"

#include <assert.h>
#include <string.h>

#include <stdexcept>
#include "coda_oss/memory.h"
#include "coda_oss/span.h"

#include "str/Convert.h"
#include "str/Encoding.h"

template <typename CharT>
inline coda_oss::span<const char> make_span(const CharT* s)
{
    // Be sure we can cast between the different types
    static_assert(sizeof(*s) == sizeof(std::string::value_type), "wrong size for CharT");
    static_assert(sizeof(*s) == sizeof(sys::U8string::value_type), "wrong size for CharT");
    static_assert(sizeof(*s) == sizeof(str::W1252string::value_type), "wrong size for CharT");
    auto s_ = str::cast<const char*>(s);
    return coda_oss::span<const char>(s_, strlen(s_));
}

template<typename CharT>
inline coda_oss::span<const char> make_span(const std::basic_string<CharT>& s)
{
    // Be sure we can cast between the different types
    static_assert(sizeof(s[0]) == sizeof(std::string::value_type), "wrong size for CharT");
    static_assert(sizeof(s[0]) == sizeof(sys::U8string::value_type), "wrong size for CharT");
    static_assert(sizeof(s[0]) == sizeof(str::W1252string::value_type), "wrong size for CharT");
    return coda_oss::span<const char>(str::c_str<const char*>(s), s.size());
}

static std::string utf8_to_native(coda_oss::span<const char> s)
{
    std::string retval;
    str::details::toString(str::cast<sys::U8string::const_pointer>(s.data()), retval);
    return retval;
}
static std::string w1252_to_native(coda_oss::span<const char> s)
{
    // This internal helper routine will convert from Windows-1252 to UTF-8 on Linux
    std::string retval;
    str::details::toNative(str::cast<str::W1252string::const_pointer>(s.data()), retval);
    return retval;
}

inline sys::U8string utf8_to_u8string(coda_oss::span<const char> s)
{
    return str::cast<sys::U8string::const_pointer>(s.data());
}
inline sys::U8string w1252_to_u8string(coda_oss::span<const char> s)
{
    return str::to_u8string(str::cast<str::W1252string::const_pointer>(s.data()), s.size());
}

struct str::EncodedStringView::Impl final
{
    // Since we only support two encodings--UTF-8 (native on Linux) and Windows-1252
    // (native on Windows)--both of which are 8-bits, a simple "bool" flag will do.
    coda_oss::span<const char> mString;
    static constexpr bool mNativeIsUtf8 = str::details::Platform == str::details::PlatformType::Linux ? true : false;
    bool mIsUtf8 = mNativeIsUtf8;

    Impl() = default;
    Impl(std::string::const_pointer p) : mString(make_span(p)) { }
    Impl(sys::U8string::const_pointer p) : mString(make_span(p)), mIsUtf8(true) { }
    Impl(str::W1252string::const_pointer p) : mString(make_span(p)), mIsUtf8(false) { }
    Impl(const std::basic_string<std::string::value_type>& s) : mString(make_span(s)){ }
    Impl(const std::basic_string<sys::U8string::value_type>& s) : mString(make_span(s)), mIsUtf8(true) { }
    Impl(const std::basic_string<str::W1252string::value_type>& s) : mString(make_span(s)), mIsUtf8(false) { }

    std::string native() const
    {
        return mIsUtf8 ? utf8_to_native(mString) : w1252_to_native(mString);
    }

    sys::U8string to_u8string() const
    {
        return mIsUtf8 ? utf8_to_u8string(mString) : w1252_to_u8string(mString);
    }

    bool operator_eq(const Impl& rhs) const
    {
        auto& lhs = *this;

        // If all the pointers are all the same, the views must be equal
        if ((lhs.mString.data() == rhs.mString.data())
            && (lhs.mString.size() == rhs.mString.size())
            && (lhs.mIsUtf8 == rhs.mIsUtf8))
        {
            return true;
        }
    
        // if encoding is the same, strcmp() will work
        if (lhs.mIsUtf8 == rhs.mIsUtf8)
        {
            return strcmp(lhs.mString.data(), rhs.mString.data()) == 0;
        }

        // LHS and RHS have different encodings
        if (!rhs.mIsUtf8)
        {
            assert(lhs.mIsUtf8);  // should have used strcmp(), aboe
           
            if (mNativeIsUtf8)
            {
                return str::cast<str::U8string::const_pointer>(lhs.mString.data()) == rhs.to_u8string();
            }
            else
            {
                return lhs.native() == rhs.mString.data();
            }            
        }

        return rhs.operator_eq(lhs);
    }
};

str::EncodedStringView::EncodedStringView() : pImpl(coda_oss::make_unique<Impl>()) { }
str::EncodedStringView::~EncodedStringView() = default;
str::EncodedStringView::EncodedStringView(EncodedStringView&&) noexcept = default;
str::EncodedStringView& str::EncodedStringView::operator=(EncodedStringView&&) noexcept = default;

str::EncodedStringView& str::EncodedStringView::operator=(const EncodedStringView& other)
{
    this->pImpl = coda_oss::make_unique<Impl>(*other.pImpl);
    return *this;
}
str::EncodedStringView::EncodedStringView(const EncodedStringView& other)
{
    *this = other;
}

str::EncodedStringView::EncodedStringView(std::string::const_pointer p) : pImpl(coda_oss::make_unique<Impl>(p)) { }
str::EncodedStringView::EncodedStringView(sys::U8string::const_pointer p) : pImpl(coda_oss::make_unique<Impl>(p)){ }
str::EncodedStringView::EncodedStringView(str::W1252string::const_pointer p) :  pImpl(coda_oss::make_unique<Impl>(p)) { }
str::EncodedStringView::EncodedStringView(const std::string& s) : pImpl(coda_oss::make_unique<Impl>(s)) { }
str::EncodedStringView::EncodedStringView(const sys::U8string& s) : pImpl(coda_oss::make_unique<Impl>(s)) { }
str::EncodedStringView::EncodedStringView(const str::W1252string& s) : pImpl(coda_oss::make_unique<Impl>(s)) { }

std::string str::EncodedStringView::native() const
{
    return pImpl->native();
}

sys::U8string str::EncodedStringView::to_u8string() const
{
    return pImpl->to_u8string();
}
std::string& str::EncodedStringView::toUtf8(std::string& result) const
{
    // This is easy, but creates "unneeded" sys::U8string; it would be
    // better to put the result directly into std::string
    const auto utf8 = to_u8string(); // TODO: avoid this copy
    result = str::c_str<std::string::const_pointer>(utf8);
    return result;
}

bool str::EncodedStringView::operator_eq(const EncodedStringView& rhs) const
{
    return pImpl->operator_eq(*(rhs.pImpl));
}


namespace str
{
// GCC wants specializations outside of the class.  We need these here (now)
// anyway for access to pImpl.
template <>
std::string::const_pointer EncodedStringView::cast() const
{
    return pImpl->mString.data();
}
template <>
sys::U8string::const_pointer EncodedStringView::cast() const
{
    return pImpl->mIsUtf8 ? str::cast<sys::U8string::const_pointer>(pImpl->mString.data()) : nullptr;
}
template <>
str::W1252string::const_pointer EncodedStringView::cast() const
{
    return pImpl->mIsUtf8 ? nullptr : str::cast<str::W1252string::const_pointer>(pImpl->mString.data());
}
} // namespace str

