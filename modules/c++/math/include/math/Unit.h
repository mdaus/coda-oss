/* =========================================================================
 * This file is part of math-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2015, MDA Information Systems LLC
 * (C) Copyright 2021, Maxar Technologies, Inc.
 *
 * math-c++ is free software; you can redistribute it and/or modify
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
#ifndef CODA_OSS_math_Unit_h_INCLUDED_
#define CODA_OSS_math_Unit_h_INCLUDED_

#include <type_traits>

namespace math
{
//
// Avoid confusion between different units
//
template <typename T, typename Tag, typename bool = std::enable_if<std::is_arithmetic<T>::value>>
struct Unit final
{
    T value_;
    T& value()
    {
        return value_;
    }
    const T& value() const
    {
        return value_;
    }
};
}

#endif  // CODA_OSS_math_Unit_h_INCLUDED_
