/* =========================================================================
 * This file is part of math-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * © Copyright 2023, Maxar Technologies, Inc.
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
#ifndef CODA_OSS_math_Trig_h_INCLUDED_
#define CODA_OSS_math_Trig_h_INCLUDED_

#include "config/Exports.h"

#include "Span.h"
#include "execution_policy.h"

namespace math
{

// "Overloads" for common routines in <math.h>

CODA_OSS_API void Sin(execution_policy, span<const float>, span<float>);
CODA_OSS_API void Sin(execution_policy, span<const double>, span<double>);

CODA_OSS_API void Cos(execution_policy, span<const float>, span<float>);
CODA_OSS_API void Cos(execution_policy, span<const double>, span<double>);

//CODA_OSS_API void SinCos(execution_policy, span<const float>, span<float> sines, span<float> cosines);
//CODA_OSS_API void SinCos(execution_policy, span<const double>, span<double> sines, span<double> cosines);

CODA_OSS_API void Tan(execution_policy, span<const float>, span<float>);
CODA_OSS_API void Tan(execution_policy, span<const double>, span<double>);

//CODA_OSS_API void ATan2(execution_policy, span<const float>, span<const float>, span<float>);
//CODA_OSS_API void ATan2(execution_policy, span<const double>, span<const double>, span<double>);

}

#endif  // CODA_OSS_math_Trig_h_INCLUDED_
