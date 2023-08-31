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

#include <execution>

#include "Span.h"
#include "ExecutionPolicy.h"

namespace math
{

// "Overloads" for common math routines
CODA_OSS_API void Sin(ExecutionPolicy, span<const float>, span<float>);
CODA_OSS_API void Sin(ExecutionPolicy, span<const double>, span<double>);
template<typename TExecutionPolicy, typename T>
inline void Sin(TExecutionPolicy&& policy, span<const T> values, span<T> results)
{
    Sin(to_ExecutionPolicy(policy), values, results);
}
template <typename T>
inline void Sin(span<const T> values, span<T> results)
{
    Sin(ExecutionPolicy::None, values, results);
}

//CODA_OSS_API void Cos(ExecutionPolicy, span<const float>, span<float>);
//CODA_OSS_API void Cos(ExecutionPolicy, span<const double>, span<double>);
//
////CODA_OSS_API void SinCos(ExecutionPolicy, span<const float>, span<float> sines, span<float> cosines);
////CODA_OSS_API void SinCos(ExecutionPolicy, span<const double>, span<double> sines, span<double> cosines);
//
//CODA_OSS_API void Tan(ExecutionPolicy, span<const float>, span<float>);
//CODA_OSS_API void Tan(ExecutionPolicy, span<const double>, span<double>);
//
////CODA_OSS_API void ATan2(ExecutionPolicy, span<const float>, span<const float>, span<float>);
////CODA_OSS_API void ATan2(ExecutionPolicy, span<const double>, span<const double>, span<double>);

}

#endif  // CODA_OSS_math_Trig_h_INCLUDED_
