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

#include <stdexcept>

#include "str/Convert.h"
#include "str/Encoding.h"

str::EncodedStringView::EncodedStringView(const std::string& s) : mpString(&s)
{
}
str::EncodedStringView::EncodedStringView(const sys::U8string& s) : mpU8String(&s)
{
}
str::EncodedStringView::EncodedStringView(const str::W1252string& s) : mpW1252String(&s)
{
}

size_t str::EncodedStringView::size() const
{
    return mpString != nullptr ? mpString->size() :
        (mpU8String != nullptr ? mpU8String->size() : mpW1252String->size());
}

const char* str::EncodedStringView::c_str() const
{
    if (mpString != nullptr)
    {
        assert(mpU8String == nullptr);
        assert(mpW1252String == nullptr);
        return mpString->c_str();
    }
    if (mpU8String != nullptr)
    {
        assert(mpString == nullptr);
        assert(mpW1252String == nullptr);
        return str::c_str<const char*>(*mpU8String);
    }
    if (mpW1252String != nullptr)
    {
        assert(mpString == nullptr);
        assert(mpU8String == nullptr);
        return str::c_str<const char*>(*mpW1252String);
    }

    throw std::logic_error("Can't determine c_str() result");
}

std::string str::EncodedStringView::native() const
{
    if (mpString != nullptr)
    {
        assert(mpU8String == nullptr);
        assert(mpW1252String == nullptr);
        return *mpString; // easy-peasy
    }

    // For a UTF-8 string, str::toString() will convert; see the specialization in Encoding.cpp
    if (mpU8String != nullptr)
    {
        assert(mpString == nullptr);
        assert(mpW1252String == nullptr);
        return toString(*mpU8String);
    }

    // This internal helper routine will convert from Windows-1252 to UTF-8 on Linux
    if (mpW1252String != nullptr)
    {
        assert(mpString == nullptr);
        assert(mpU8String == nullptr);

        std::string retval;
        str::details::toNative(*mpW1252String, retval);
        return retval;
    }

    throw std::logic_error("Can't determine native() result");
}

sys::U8string str::EncodedStringView::to_u8string() const
{
    if (mpString != nullptr)
    {
        assert(mpU8String == nullptr);
        assert(mpW1252String == nullptr);
        return str::to_u8string(*mpString);
    }
    if (mpU8String != nullptr)
    {
        assert(mpString == nullptr);
        assert(mpW1252String == nullptr);
        return str::to_u8string(*mpU8String);
    }
    if (mpW1252String != nullptr)
    {
        assert(mpString == nullptr);
        assert(mpU8String == nullptr);
        return str::to_u8string(*mpW1252String);
    }

    throw std::logic_error("Can't determine to_u8string() result");
}
std::string& str::EncodedStringView::toUtf8(std::string& result) const
{
    // This is easy, but creates "unneeded" sys::U8string; it would be
    // better to put the result directly into std::string
    const auto utf8 = to_u8string(); // TODO: avoid this copy
    result = str::c_str<std::string::const_pointer>(utf8);
    return result;
}