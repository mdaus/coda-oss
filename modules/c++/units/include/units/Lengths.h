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
#ifndef CODA_OSS_units_Lengths_h_INCLUDED_
#define CODA_OSS_units_Lengths_h_INCLUDED_

#include <cmath>

#include "math/Constants.h"
#include "units/Unit.h"

namespace units
{
//
// Avoid confusion between feet and meters
//
namespace tags
{
struct Feet final { };
struct Meters final { };
}

template <typename T, typename Tag>
using Length = Unit<T, Tag>;

template <typename T>
using Feet = Length<T, tags::Feet>;
template <typename T>
inline Feet<T> asFeet(T a) noexcept
{
    return Feet<T>{a};
}
template <typename T, typename TReturn = T>
inline Feet<TReturn> toFeet(Feet<T> a) noexcept
{
    return Feet<TReturn>{a.value()};
}

template <typename T>
using Meters = Length<T, tags::Meters>;
template <typename T>
inline Meters<T> asMeters(T a) noexcept
{
    return Meters<T>{a};
}
template <typename T, typename TReturn = T>
inline Meters<TReturn> toMeters(Meters<T> a) noexcept
{
    return Meters<TReturn>{a.value()};
}

template <typename T, typename TReturn = T>
inline Feet<TReturn> toFeet(Meters<T> a) noexcept
{
    return Feet<TReturn>{a.value() * math::Constants::METERS_TO_FEET};
}
template <typename T, typename TReturn = T>
inline Meters<TReturn> toMeters(Feet<T> a) noexcept
{
    return Meters<TReturn>{a.value() * math::Constants::FEET_TO_METERS};
}


namespace tags
{
struct NauticalMiles final { };
}

template <typename T>
using NauticalMiles = Length<T, tags::NauticalMiles>;
template <typename T>
inline NauticalMiles<T> asNauticalMiles(T v) noexcept
{
    return NauticalMiles<T>{v};
}
template <typename T, typename TReturn = T>
inline NauticalMiles<TReturn> toNauticalMiles(NauticalMiles<T> v) noexcept
{
    return NauticalMiles<TReturn>{v.value()};
}

template <typename T, typename TReturn = T>
inline Feet<TReturn> toFeet(NauticalMiles<T> v) noexcept
{
    return Feet<TReturn>{v.value() * math::Constants::NAUTICAL_MILES_TO_FEET};
}
template <typename T, typename TReturn = T>
inline Meters<TReturn> toMeters(NauticalMiles<T> v) noexcept
{
    return Meters<TReturn>{v.value() * math::Constants::NAUTICAL_MILES_TO_METERS};
}
template <typename T, typename TReturn = T>
inline NauticalMiles<TReturn> toNauticalMiles(Meters<T> l) noexcept
{
    return NauticalMiles<TReturn>{v.value() * math::Constants::METERS_TO_NAUTICAL_MILES};
}

}

#endif  // CODA_OSS_units_Lengths_h_INCLUDED_
