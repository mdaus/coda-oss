/* =========================================================================
 * This file is part of simd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * (C) Copyright 2023, Maxar Technologies, Inc.
 *
 * simd--c++ is free software; you can redistribute it and/or modify
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
#ifndef CODA_OSS_simd_Math_h_INCLUDED_
#define CODA_OSS_simd_Math_h_INCLUDED_

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES // get M_PI, etc.
#endif
#include <math.h>
#include <stdint.h>

#include "coda_oss/CPlusPlus.h"
#include "coda_oss/span.h"
#include "config/Exports.h"

namespace simd
{

// "Overloads" for common routines in <math.h> to support SIMD processing,
// although clients aren't (too) aware of the implementation.
CODA_OSS_API void Sin(coda_oss::span<const float>, coda_oss::span<float>);
CODA_OSS_API void Sin(coda_oss::span<const double>, coda_oss::span<double>);
CODA_OSS_API void Cos(coda_oss::span<const float>, coda_oss::span<float>);
CODA_OSS_API void Tan(coda_oss::span<const float>, coda_oss::span<float>);

}

#endif  // CODA_OSS_simd_Math_h_INCLUDED_
