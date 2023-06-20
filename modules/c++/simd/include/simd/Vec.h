/* =========================================================================
 * This file is part of types-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * (C) Copyright 2023, Maxar Technologies, Inc.
 *
 * types-c++ is free software; you can redistribute it and/or modify
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
#ifndef CODA_OSS_simd_Vec_h_INCLUDED_
#define CODA_OSS_simd_Vec_h_INCLUDED_

#include <stdint.h>

#include "coda_oss/CPlusPlus.h"
#include "import/vectorclass.h"

namespace simd
{

// Templated wrapper around the various vcl::Vec classes
// https://github.com/vectorclass/manual/raw/master/vcl_manual.pdf
namespace details
{
template <typename T, size_t elements_per_vector>
struct Vec;

template <>
struct Vec<int8_t, 16> final
{
    using type = vcl::Vec16c;
};

template <>
struct Vec<float, 4> final
{
    using type = vcl::Vec4f;
};
template <>
struct Vec<float, 8> final
{
    using type = vcl::Vec8f;
};
template <>
struct Vec<float, 16> final
{
    using type = vcl::Vec16f;
};
}

template <typename T, size_t elements_per_vector>
using Vec = typename details::Vec<T, elements_per_vector>::type;

}

#endif  // CODA_OSS_simd_Vec_h_INCLUDED_
