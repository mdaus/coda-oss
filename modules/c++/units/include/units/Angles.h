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
namespace tags
{
struct Radians final { };
struct Degrees final { };
}

template <typename T, typename Tag>
using Angle = Unit<T, Tag>;

template <typename T>
using Radians = Angle<T, tags::Radians>;
template <typename T>
inline Radians<T> asRadians(T v) noexcept
{
    return as<tags::Radians>(v);
}

template <typename T>
using Degrees = Angle<T, tags::Degrees>;
template <typename T>
inline Degrees<T> asDegrees(T v) noexcept
{
    return as<tags::Degrees>(v);
}

template <typename Tag, typename T, typename TReturn = T>
inline Radians<TReturn> to(Degrees<T> v) noexcept
{
    return Radians<TReturn>(v.value() * math::Constants::DEGREES_TO_RADIANS);
}
template <typename Tag, typename T, typename TReturn = T>
inline Degrees<TReturn> to(Radians<T> v) noexcept
{
    return Degrees<TReturn>(v.value() * math::Constants::RADIANS_TO_DEGREES);
}

template <typename T, typename Tag>
inline T sin(Angle<T, Tag> v) noexcept
{
    return std::sin(to<tags::Radians>(v).value());
}
template <typename T, typename Tag>
inline T cos(Angle<T, Tag> v) noexcept
{
    return std::cos(to<tags::Radians>(v).value());
}
template <typename T, typename Tag>
inline T tan(Angle<T, Tag> v) noexcept
{
    return std::tan(to<tags::Radians>(v).value());
}
}

#endif  // CODA_OSS_units_Angles_h_INCLUDED_
