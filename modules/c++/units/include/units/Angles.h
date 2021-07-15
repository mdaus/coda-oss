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
#ifndef CODA_OSS_units_Angles_h_INCLUDED_
#define CODA_OSS_units_Angles_h_INCLUDED_

#include <cmath>

#include "math/Constants.h"
#include "units/Unit.h"

namespace units
{
//
// Avoid confusion between degrees and radians
//
namespace details
{
namespace tags
{
struct Radians final { };
struct Degrees final { };
}
}

template <typename T, typename Tag>
using Angle = Unit<T, Tag>;

template <typename T>
using Radians = Angle<T, details::tags::Radians>;
template <typename T>
inline Radians<T> asRadians(T a) noexcept
{
    return Radians<T>{a};
}
template <typename T, typename TReturn = T>
inline Radians<TReturn> toRadians(Radians<T> a) noexcept
{
    return Radians<TReturn>{a.value()};
}

template <typename T>
using Degrees = Angle<T, details::tags::Degrees>;
template <typename T>
inline Degrees<T> asDegrees(T a) noexcept
{
    return Degrees<T>{a};
}
template <typename T, typename TReturn = T>
inline Degrees<TReturn> toDegrees(Degrees<T> a) noexcept
{
    return Degrees<TReturn>{a.value()};
}

template <typename T, typename TReturn = T>
inline Radians<TReturn> toRadians(Degrees<T> a) noexcept
{
    return Radians<TReturn>{a.value() * math::Constants::DEGREES_TO_RADIANS};
}
template <typename T, typename TReturn = T>
inline Degrees<TReturn> toDegrees(Radians<T> a) noexcept
{
    return Degrees<TReturn>{a.value() * math::Constants::RADIANS_TO_DEGREES};
}

template <typename T, typename Tag>
inline T sin(Angle<T, Tag> a) noexcept
{
    return std::sin(toRadians(a).value());
}
template <typename T, typename Tag>
inline T cos(Angle<T, Tag> a) noexcept
{
    return std::cos(toRadians(a).value());
}
template <typename T, typename Tag>
inline T tan(Angle<T, Tag> a) noexcept
{
    return std::tan(toRadians(a).value());
}
}

#endif  // CODA_OSS_units_Angles_h_INCLUDED_
