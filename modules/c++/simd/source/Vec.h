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
template <size_t elements_per_vector, typename T>
struct Vec;

template <>
struct Vec<16, int8_t> final
{
    using type = vcl::Vec16c;
};

template <>
struct Vec<4, float> final
{
    using type = vcl::Vec4f;
};
template <>
struct Vec<8, float> final
{
    using type = vcl::Vec8f;
};
template <>
struct Vec<16, float> final
{
    using type = vcl::Vec16f;
};

template <size_t elements_per_vector>
using Vecf = typename Vec<elements_per_vector, float>::type;

template <>
struct Vec<2, double> final
{
    using type = vcl::Vec2d;
};
template <>
struct Vec<4, double> final
{
    using type = vcl::Vec4d;
};
template <>
struct Vec<8, double> final
{
    using type = vcl::Vec8d;
};
template <size_t elements_per_vector>
using Vecd = typename Vec<elements_per_vector, double>::type;

template<typename T>
using Vec1 =  typename Vec<1, T>::type;
template<typename T>
using Vec2 =  typename Vec<2, T>::type;
template<typename T>
using Vec4 =  typename Vec<4, T>::type;
template<typename T>
using Vec8 =  typename Vec<8, T>::type;
template<typename T>
using Vec16 =  typename Vec<16, T>::type;

} // namespace "details"

template <size_t elements_per_vector, typename T>
using Vec_t = typename details::Vec<elements_per_vector, T>::type;

// load() and store() overloads for meta-programming.
template <typename TVec, typename T>
inline void load(TVec& vec, span<const T> values, size_t i)
{
    vec.load(&(values[i]));  // load_a() requires very strict alignment
}
template <typename TVec, typename T>
inline void load_partial(TVec& vec, int n, span<const T> values, size_t i)
{
    vec.load_partial(n, &(values[i]));
}
template <typename TVec, typename T>
inline void store(const TVec& vec, span<T> results, size_t i)
{
    vec.store(&(results[i]));  // store_a() requires very strict alignment
}
template <typename TVec, typename T>
inline void store_partial(const TVec& vec, int n, span<T> results, size_t i)
{
    vec.store_partial(n, &(results[i]));
}

}

#endif  // CODA_OSS_simd_Vec_h_INCLUDED_
