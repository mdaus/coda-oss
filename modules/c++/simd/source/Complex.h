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
#ifndef CODA_OSS_simd_Complex_h_INCLUDED_
#define CODA_OSS_simd_Complex_h_INCLUDED_

#include <stdint.h>

#include "coda_oss/CPlusPlus.h"
#include "import/vectorclass.h"

namespace simd
{

// Templated wrapper around the various vcl::Complex classes
// https://github.com/vectorclass/manual/raw/master/vcl_manual.pdf
namespace details
{
template <size_t elements_per_complex, typename T>
struct Complex;

template <>
struct Complex<1, float> final
{
    using type = vcl::Complex1f;
};
template <>
struct Complex<2, float> final
{
    using type = vcl::Complex2f;
};
template <>
struct Complex<4, float> final
{
    using type = vcl::Complex4f;
};
template <>
struct Complex<8, float> final
{
    using type = vcl::Complex8f;
};

template <>
struct Complex<1, double> final
{
    using type = vcl::Complex1d;
};
template <>
struct Complex<2, double> final
{
    using type = vcl::Complex2d;
};
template <>
struct Complex<4, double> final
{
    using type = vcl::Complex4d;
};

} // namespace "details"

template <size_t elements_per_complex, typename T>
using Complex_t = typename details::Complex<elements_per_complex, T>::type;

// load() and store() overloads for meta-programming.
template <size_t width, typename T, typename TValue = typename T::value_type>
inline void load(simd::Complex_t<width, TValue>& cx, span<const T> values, size_t i)
{
    const void* const pValues = &(values[i]);
    cx.load(static_cast<const TValue*>(pValues));
}
template <size_t width, typename T, typename TValue = typename T::value_type>
inline void load_partial(simd::Complex_t<width, TValue>& cx, int n, span<const T> values, size_t i)
{
    for (int j = 0; j < n; j++)
    {
        auto&& value = values[i + j];
        cx.insert(j, simd::Complex_t<1, TValue>(value.real(), value.imag()));    
    }
}
template <size_t width, typename T, typename TValue = typename T::value_type>
inline void store(const simd::Complex_t<width, TValue>& cx, span<T> results, size_t i)
{
    void* const pResults = &(results[i]);
    cx.store(static_cast<TValue*>(pResults));
}
template <size_t width, typename T, typename TValue = typename T::value_type>
inline void store_partial(const simd::Complex_t<width, TValue>& cx, int n, span<T> results_, size_t i)
{
    for (int j = 0; j < n; j++)
    {
        auto results = sys::make_span(results_.data() + j, 1);
        store(cx.extract(j), results, 0);
    }
}

}

#endif  // CODA_OSS_simd_Complex_h_INCLUDED_