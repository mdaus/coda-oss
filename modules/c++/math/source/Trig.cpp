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

#include "math/Trig.h"

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES // get M_PI, etc.
#endif
#include <math.h>

#include <stdexcept>
#include <functional>
#include <algorithm>
#include <execution>
#include <tuple> // std::ignore
#include <cmath>

#include "sys/OS.h"

template<typename T>
using span = math::span<T>;

// "Figure out" an `ExecutionPolicy` based on  `getSIMDInstructionSet()`.
static auto determineExecutionPolicy()
{
    static const auto simdInstructionSet = sys::OS().getSIMDInstructionSet();
    switch (simdInstructionSet)
    {
    case sys::SIMDInstructionSet::SSE2:
        return math::ExecutionPolicy::ParUnseq;

    // Auto-vectorization seems to work pretty well, at least for `sin()`, et. al. below
    case sys::SIMDInstructionSet::AVX2:
    case sys::SIMDInstructionSet::AVX512F:
        return math::ExecutionPolicy::None;

    default:
        throw std::logic_error("Unknown SIMDInstructionSet value.");
    }
}

// https://en.cppreference.com/w/cpp/algorithm/transform
template <typename ExecutionPolicy, typename T, typename BinaryOperation>
inline auto transform(ExecutionPolicy&& policy,  span<const T> values, span<T> results,
    BinaryOperation binary_op)
{
    return std::transform(policy, values.begin(), values.end(),  results.begin(), binary_op);
}
template <typename T, typename BinaryOperation>
inline auto transform(span<const T> values, span<T> results,
    BinaryOperation binary_op)
{
    return std::transform(values.begin(), values.end(),  results.begin(), binary_op);
}

template <typename T, typename BinaryOperation>
inline void transform(math::ExecutionPolicy policy, 
    span<const T> values, span<T> results, BinaryOperation binary_op)
{
    switch (policy)
    {
    case math::ExecutionPolicy::Seq:
        std::ignore = transform(std::execution::seq, values, results, binary_op);
        break;
    
    case math::ExecutionPolicy::Par:
        std::ignore = transform(std::execution::par, values, results, binary_op);
        break;
    
    case math::ExecutionPolicy::ParUnseq:
        std::ignore = transform(std::execution::par_unseq, values, results, binary_op);
        break;

    case math::ExecutionPolicy::Unseq:
        #if CODA_OSS_cpp20
        std::ignore = transform(std::execution::unseq, values, results, binary_op);
        #else
        throw std::logic_error("C++20 needed for `Unseq`.");
        #endif
        break;

    case math::ExecutionPolicy::None:
        std::ignore = transform(values, results, binary_op);
        break;

    case math::ExecutionPolicy::Unknown:
        transform(determineExecutionPolicy(), values, results, binary_op);
        break;
    }
}

void math::Sin(ExecutionPolicy policy, span<const float> values, span<float> results)
{
    transform(policy, values, results, sinf);
}
inline double sind(double arg)
{
    return ::sin(arg);
}
void math::Sin(ExecutionPolicy policy, span<const double> values, span<double> results)
{
    transform(policy, values, results, sind);
}

//void math::Cos(ExecutionPolicy policy, span<const float> values, span<float> results)
//{
//    static const auto f = [](auto&& v) { return std::cos(v); };
//    transform(policy, values, results, f);
//}
//void math::Cos(ExecutionPolicy policy, span<const double> values, span<double> results)
//{
//    static const auto f = [](auto&& v) { return std::cos(v); };
//    transform(policy, values, results, f);
//}
//
//void math::Tan(ExecutionPolicy policy, span<const float> values, span<float> results)
//{
//    static const auto f = [](auto&& v) { return std::tan(v); };
//    transform(policy, values, results, f);
//}
//void math::Tan(ExecutionPolicy policy, span<const double> values, span<double> results)
//{
//    static const auto f = [](auto&& v) { return std::tan(v); };
//    transform(policy, values, results, f);
//}
