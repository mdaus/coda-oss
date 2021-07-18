/* =========================================================================
 * This file is part of units-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2015, MDA Information Systems LLC
 * (C) Copyright 2021, Maxar Technologies, Inc.
 *
 * units-c++ is free software; you can redistribute it and/or modify
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
#ifndef CODA_OSS_units_Unit_h_INCLUDED_
#define CODA_OSS_units_Unit_h_INCLUDED_

#include <type_traits>

namespace units
{
//
// Simple template to help avoid confusion between different units.
// 
// This is intentionally simple for several reasons:
// * simple and easy to understand and maintain
// * getting things like "+" or "*" right can be (very) difficult
// * it's not even clear that doubling a temperature has real meaning
// * there's really not that much code that needs to manipulate units
//
template <typename Tag, typename T>
struct Unit final
{
    using tag_t = Tag;
    using value_t = T;

    value_t value_;

    Unit() = delete;
    Unit(value_t v) noexcept : value_(v) { }

    value_t& value() noexcept
    {
        return value_;
    }
    const value_t& value() noexcept const
    {
        return value_;
    }
};

template<typename Tag, typename T>
inline Unit<Tag, T> as(T t) noexcept
{
    return Unit<Tag, T>(t);
}


template <typename Tag, typename T, typename TReturn = T>
inline Unit<Tag, TReturn> to(Unit<Tag, T> v) noexcept
{
    return Unit<Tag, TReturn>(v.value());
}

}

#endif  // CODA_OSS_units_Unit_h_INCLUDED_
