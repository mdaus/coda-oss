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
#ifndef CODA_OSS_math_ExecutionPolicy_h_INCLUDED_
#define CODA_OSS_math_ExecutionPolicy_h_INCLUDED_

#include <execution>

#include "coda_oss/CPlusPlus.h"

namespace math
{

// C++17 does **not** put these in an `enum`, presumablly to make it easier
// for implementations/libraries to add their own values (e.g., GPU processing).
// We don't have that concern, and a concrete type (rather than a template)
// means we can put the implementation in a CPP file.

// https://en.cppreference.com/w/cpp/algorithm/execution_policy_tag
enum class ExecutionPolicy
{
	Seq, Par, ParUnseq,
    #if CODA_OSS_cpp20
    Unseq
    #endif
};

template<typename TExecutionPolicy>
constexpr auto to_ExecutionPolicy(TExecutionPolicy&& policy)
{
    if constexpr (policy == std::execution::seq) return ExecutionPolicy::Seq;
    if constexpr (policy == std::execution::par) return ExecutionPolicy::Par;
    if constexpr (policy == std::execution::par_unseq) return ExecutionPolicy::ParUnseq;
    #if CODA_OSS_cpp20
    if constexpr (policy == std::execution::unseq) return ExecutionPolicy::Unseq;
    #endif
    static_assert("should not get here!");
}

}

#endif  // CODA_OSS_math_ExecutionPolicy_h_INCLUDED_
