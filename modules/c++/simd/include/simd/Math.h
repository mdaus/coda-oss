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

#include <complex>

#include "coda_oss/CPlusPlus.h"
#include "coda_oss/span.h"
#include "config/Exports.h"

#include "Span.h"

namespace simd
{

// "Overloads" for common routines in <math.h> to support SIMD processing,
// although clients aren't (too) aware of the implementation.
CODA_OSS_API void Sin(span<const float>, span<float>);
CODA_OSS_API void Sin(span<const double>, span<double>);

CODA_OSS_API void Cos(span<const float>, span<float>);
CODA_OSS_API void Cos(span<const double>, span<double>);

CODA_OSS_API void SinCos(span<const float>, span<float> sines, span<float> cosines);
CODA_OSS_API void SinCos(span<const double>, span<double> sines, span<double> cosines);

CODA_OSS_API void Tan(span<const float>, span<float>);
CODA_OSS_API void Tan(span<const double>, span<double>);

CODA_OSS_API void ATan2(span<const float>, span<const float>, span<float>);
CODA_OSS_API void ATan2(span<const double>, span<const double>, span<double>);

}

#endif  // CODA_OSS_simd_Math_h_INCLUDED_
