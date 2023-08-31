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

template<typename T>
using span = math::span<T>;

// https://en.cppreference.com/w/cpp/algorithm/transform
template <typename ExecutionPolicy, typename T, typename BinaryOperation>
inline auto transform(ExecutionPolicy&& policy,  span<const T> values, span<T> results,
    BinaryOperation binary_op)
{
    return std::transform(policy, values.begin(), values.end(),  results.begin(), binary_op);
}

template <typename T, typename BinaryOperation>
inline void transform(math::execution_policy policy, 
    span<const T> values, span<T> results, BinaryOperation binary_op)
{
    switch (policy)
    {
    case math::execution_policy::seq:
        std::ignore = transform(std::execution::seq, values, results, binary_op);
        break;
    case math::execution_policy::par:
        std::ignore = transform(std::execution::par, values, results, binary_op);
        break;
    case math::execution_policy::par_unseq:
        std::ignore = transform(std::execution::par_unseq, values, results, binary_op);
        break;
    case math::execution_policy::unseq:
        #if CODA_OSS_cpp20
        std::ignore = transform(std::execution::unseq, values, results, binary_op);
        #else
        throw std::logic_error("unseq requires C++20");
        #endif
        break;
    }
}

void math::Sin(execution_policy policy, span<const float> values, span<float> results)
{
    static const auto f = [](auto&& v) { return std::sin(v); };
    transform(policy, values, results, f);
}
void math::Sin(execution_policy policy, span<const double> values, span<double> results)
{
    static const auto f = [](auto&& v) { return std::sin(v); };
    transform(policy, values, results, f);
}

void math::Cos(execution_policy policy, span<const float> values, span<float> results)
{
    static const auto f = [](auto&& v) { return std::cos(v); };
    transform(policy, values, results, f);
}
void math::Cos(execution_policy policy, span<const double> values, span<double> results)
{
    static const auto f = [](auto&& v) { return std::cos(v); };
    transform(policy, values, results, f);
}

void math::Tan(execution_policy policy, span<const float> values, span<float> results)
{
    static const auto f = [](auto&& v) { return std::tan(v); };
    transform(policy, values, results, f);
}
void math::Tan(execution_policy policy, span<const double> values, span<double> results)
{
    static const auto f = [](auto&& v) { return std::tan(v); };
    transform(policy, values, results, f);
}
