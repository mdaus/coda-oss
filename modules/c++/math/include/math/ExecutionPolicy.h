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

namespace execution
{
    // `none` (or `no_policy`) means "we've thought about it" or "did some checks"
    // and have "determiend" that the "best" execution policy is "no policy."
    // This might mean, for example, that `transform()` is called *without* an
    // `ExecutionPolicy`.
    struct no_policy final { };
    inline constexpr no_policy none{};

    // `unknown` means "we don't know what the exeuction policy should be
    // (if any);" the implementaton should figure out what to do.  This might
    // be the same as `none`, or it might use SIMD instructors or Intel MKL.
    struct unknown_policy final { };
    inline constexpr unknown_policy unknown{};
}

// https://en.cppreference.com/w/cpp/algorithm/execution_policy_tag
enum class ExecutionPolicy
{
    None, // no policy specified, easier than passing a `NULL` pointer
    Unknown, // implementation figures out what to do
	Seq, Par, ParUnseq,
    Unseq, // n.b., std::execution::unseq is C++20
};

template<typename TExecutionPolicy>
auto to_ExecutionPolicy(TExecutionPolicy&& policy)
{
    const void* pPolicy = &policy;
    if (pPolicy == &execution::none) return ExecutionPolicy::None;
    if (pPolicy == &execution::unknown) return ExecutionPolicy::Unknown;

    if (pPolicy == &std::execution::seq) return ExecutionPolicy::Seq;
    if (pPolicy == &std::execution::par) return ExecutionPolicy::Par;
    if (pPolicy == &std::execution::par_unseq) return ExecutionPolicy::ParUnseq;
    #if CODA_OSS_cpp20
    if (pPolicy == &std::execution::unseq) return ExecutionPolicy::Unseq;
    #endif
    throw std::logic_error("Unknown execution policy in to_ExecutionPolicy().");
}

}

#endif  // CODA_OSS_math_ExecutionPolicy_h_INCLUDED_
