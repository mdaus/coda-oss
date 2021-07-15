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
#ifndef CODA_OSS_math_Angles_h_INCLUDED_
#define CODA_OSS_math_Angles_h_INCLUDED_

#define _USE_MATH_DEFINES
#include <math.h>

#include <cmath>

#include "math/Constants.h"
#include "math/Unit.h"

namespace math
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

template <typename T>
using Radians = Units<T, details::tags::Radians>;
template <typename T>
inline Radians<T> asRadians(T a)
{
    return Radians<T>{a};
}
template <typename T, typename TReturn = T>
inline Radians<TReturn> toRadians(Radians<T> a)
{
    return Radians<TReturn>{a.value};
}

template <typename T>
using Degrees = Units<T, details::tags::Degrees>;
template <typename T>
inline Degrees<T> asdegrees(T a)
{
    return Degrees<T>{a};
}
template <typename T, typename TReturn = T>
inline Degrees<TReturn> toDegrees(Degrees<T> a)
{
    return Degrees<TReturn>{a.value};
}

template <typename T, typename TReturn = T>
inline Radians<TReturn> toRadians(Degrees<T> a)
{
    return Radians<TReturn>{a.value * Constants::DEGREES_TO_RADIANS};
}
template <typename T, typename TReturn = T>
inline Degrees<TReturn> toDegrees(Radians<T> a)
{
    return Degrees<TReturn>{a.value * Constants::RADIANS_TO_DEGREES};
}

template <template <typename> typename TAngle, typename T>
inline T sin(TAngle<T> a)
{
    return std::sin(toRadians(a).value);
}
template <template <typename> typename TAngle, typename T>
inline T cos(TAngle<T> a)
{
    return std::cos(toRadians(a).value);
}
template <template <typename> typename TAngle, typename T>
inline T tan(TAngle<T> a)
{
    return std::tan(toRadians(a).value);
}
inline double sin(double a)
{
    return std::sin(a);
}
inline double cos(double a)
{
    return std::cos(a);
}
inline double tan(double a)
{
    return std::tan(a);
}
}

#endif  // CODA_OSS_math_Angles_h_INCLUDED_
