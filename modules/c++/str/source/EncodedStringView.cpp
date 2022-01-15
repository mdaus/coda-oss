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

#include "str/Convert.h"
#include "str/Encoding.h"

// https://en.cppreference.com/w/cpp/string/basic_string_view
template <typename CharT>
class BasicStringView final
{
    size_t sz = 0;
    const CharT* p = nullptr;

    static size_t strlen_(const CharT* s)
    {
        return strlen(str::cast<const char*>(s));
    }

public:
    using const_pointer = const CharT*;
    BasicStringView() = default;
    BasicStringView(const CharT* s, size_t count) noexcept : p(s), sz(count) { }
    BasicStringView(const_pointer s) : BasicStringView(s, strlen_(s)) { }

    const_pointer data() const noexcept
    {
        return p;
    }

    size_t length() const noexcept
    {
        return sz;
    }

    bool empty() const noexcept
    {
        return length() == 0;
    }
};

template<typename CharT>
const char* c_str(BasicStringView<CharT> sv) noexcept
{
    return str::cast<const char*>(sv.data());
}

template<typename CharT>
BasicStringView<CharT> create_string_view(const std::basic_string<CharT>& s)
{
    return BasicStringView<CharT>(s.c_str(), s.length());
}

static std::string native(BasicStringView<sys::U8string::value_type> sv)
{
    std::string retval;
    str::details::toString(sv.data(), retval);
    return retval;
}
static std::string native(BasicStringView<str::W1252string::value_type> sv)
{
    // This internal helper routine will convert from Windows-1252 to UTF-8 on Linux
    std::string retval;
    str::details::toNative(sv.data(), retval);
    return retval;
}

struct str::EncodedStringView::Impl final
{
    BasicStringView<std::string::value_type> mString;
    BasicStringView<sys::U8string::value_type> mU8String;
    BasicStringView<str::W1252string::value_type> mW1252String;

    Impl() = default;
    Impl(std::string::const_pointer p) : mString(p) { }
    Impl(sys::U8string::const_pointer p) : mU8String(p) { }
    Impl(str::W1252string::const_pointer p) : mW1252String(p) { }
    Impl(const std::basic_string<std::string::value_type>& s) : mString(create_string_view(s)){ }
    Impl(const std::basic_string<sys::U8string::value_type>& s) : mU8String(create_string_view(s)){ }
    Impl(const std::basic_string<str::W1252string::value_type>& s) : mW1252String(create_string_view(s)){ }

    std::string native() const
    {
        // Be sure we can cast between the different types
        static_assert(sizeof(*mString.data()) == sizeof(*mU8String.data()), "wrong string sizes");
        static_assert(sizeof(*mString.data()) == sizeof(*mW1252String.data()), "wrong string sizes");
        static_assert(sizeof(*mU8String.data()) == sizeof(*mW1252String.data()), "wrong string sizes");

        std::string retval;
        if (!mString.empty())
        {
            assert(mU8String.empty());
            assert(mW1252String.empty());
            return mString.data();  // easy-peasy
        }

        if (!mU8String.empty())
        {
            assert(mString.empty());
            assert(mW1252String.empty());
            return ::native(mU8String);
        }

        if (!mW1252String.empty())
        {
            assert(mString.empty());
            assert(mU8String.empty());
            return ::native(mW1252String);
        }

        throw std::logic_error("Can't determine native() result");
    }

    sys::U8string to_u8string() const
    {
        if (!mString.empty())
        {
            assert(mU8String.empty());
            assert(mW1252String.empty());
            return str::to_u8string(mString.data(), mString.length());
        }
        if (!mU8String.empty())
        {
            assert(mString.empty());
            assert(mW1252String.empty());
            return mU8String.data(); // easy-peasy
        }
        if (!mW1252String.empty())
        {
            assert(mString.empty());
            assert(mU8String.empty());
            return str::to_u8string(mW1252String.data(), mW1252String.length());
        }

        throw std::logic_error("Can't determine to_u8string() result");
    }

    bool operator_eq(const Impl& rhs) const
    {
        auto& lhs = *this;

        // If all the pointers are all the same, the views must be equal
        if ((lhs.mString.data() == rhs.mString.data())
            && (lhs.mU8String.data() == rhs.mU8String.data())
            && (lhs.mW1252String.data() == rhs.mW1252String.data()))
        {
            assert(! ((lhs.mString.data() == nullptr) && (lhs.mU8String.data() == nullptr) && (lhs.mW1252String.data() == nullptr)) );
            return true;
        }
    
        if (!lhs.mString.empty() && !rhs.mString.empty())
        {
            assert(lhs.mU8String.empty() && rhs.mU8String.empty());
            assert(lhs.mW1252String.empty() && rhs.mW1252String.empty());
            return strcmp(::c_str(lhs.mString), ::c_str(rhs.mString)) == 0;
        }
        if (!lhs.mU8String.empty() && !rhs.mU8String.empty())
        {
            assert(lhs.mString.empty() && rhs.mString.empty());
            assert(lhs.mW1252String.empty() && rhs.mW1252String.empty());
            return strcmp(::c_str(lhs.mU8String), ::c_str(rhs.mU8String)) == 0;
        }
        if (!lhs.mW1252String.empty() && !rhs.mW1252String.empty())
        {
            assert(lhs.mString.empty() && rhs.mString.empty());
            assert(lhs.mU8String.empty() && rhs.mU8String.empty());
            return strcmp(::c_str(lhs.mW1252String), ::c_str(rhs.mW1252String)) == 0;
        }

        // LHS and RHS have different encodings
        if (!rhs.mU8String.empty()) // prefer UTF-8
        {
            // We KNOW lhs.mpU8String is NULL because of check above
            assert(lhs.mU8String.empty()); // should have used strcmp(), aboe
            return lhs.to_u8string() == rhs.mU8String.data();
        }
        if (!rhs.mString.empty()) // not UTF-8, try native
        {
            // We KNOW lhs.mpString is NULL because of check above
            assert(lhs.mString.empty());  // should have used strcmp(), aboe
            return lhs.native() == rhs.mString.data();
        }

        // One side (but not both) must be Windows-1252; convert to UTF-8 for comparison
        return lhs.to_u8string() == rhs.to_u8string();
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
template <typename TReturn, typename T2, typename T3>
typename TReturn::const_pointer EncodedStringView::cast_(const TReturn& retval, const T2& t2, const T3& t3) const
{
    if (!retval.empty())
    {
        assert(t2.empty());
        assert(t3.empty());
        return retval.data();
    }
    return nullptr;
}

// GCC wants specializations outside of the class.  We need these here (now)
// anyway for access to pImpl.
template <>
std::string::const_pointer EncodedStringView::cast() const
{
    return cast_(pImpl->mString, pImpl->mU8String, pImpl->mW1252String);
}
template <>
sys::U8string::const_pointer EncodedStringView::cast() const
{
    return cast_(pImpl->mU8String, pImpl->mString, pImpl->mW1252String);
}
template <>
str::W1252string::const_pointer EncodedStringView::cast() const
{
    return cast_(pImpl->mW1252String, pImpl->mString, pImpl->mU8String);
}
} // namespace str

