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

#include <stdexcept>

#include "str/Convert.h"

str::EncodedStringView::EncodedStringView(const sys::U8string& s) :
    mpU8String(&s)
{
}
str::EncodedStringView::EncodedStringView(const str::W1252string& s) :
    mpW1252String(&s)
{
}

size_t str::EncodedStringView::size() const
{
    return mpU8String != nullptr ? mpU8String->size() : mpW1252String->size();
}

const char* str::EncodedStringView::c_str() const
{
    if (mpU8String != nullptr)
    {
        return str::c_str<const char*>(*mpU8String);
    }
    if (mpW1252String != nullptr)
    {
        return str::c_str<const char*>(*mpW1252String);
    }

    throw std::logic_error("Can't determine c_str() result");
}
