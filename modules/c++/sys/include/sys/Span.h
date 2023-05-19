/* =========================================================================
 * This file is part of sys-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * (C) Copyright 2023, Maxar Technologies, Inc.
 *
 * sys-c++ is free software; you can redistribute it and/or modify
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

#ifndef CODA_OSS_sys_Span_h_INCLUDED_
#define CODA_OSS_sys_Span_h_INCLUDED_
#pragma once

#include <coda_oss/span.h>
#include <coda_oss/cstddef.h>
#include <vector>
#include <array>

namespace sys // not "mem", it depends on sys.
{

// Creating a `span`s is verbose w/o deduction guidelines in C++17.
// Some overloads to ease the pain.
template <typename T>
auto make_const_span(const T* ptr, size_t sz)
{
    return coda_oss::span<const T>(ptr, sz);
}
template <typename T>
auto make_span(const T* ptr, size_t sz)
{
    return make_const_span(ptr, sz);
}
template <typename T>
auto make_span(T* ptr, size_t sz)
{
    return coda_oss::span<T>(ptr, sz);
}

template <typename T>
auto make_span(coda_oss::span<T>& v) // turn span<T> into span<const T>
{
    return make_const_span(v.data(), v.size());
}


template <typename T>
auto make_span(const std::vector<T>& v)
{
    return make_span(v.data(), v.size());
}
template <typename T>
auto make_span(std::vector<T>& v)
{
    return make_span(v.data(), v.size());
}

template <typename T, size_t N>
auto make_span(const std::array<T, N>& v)
{
    return make_span(v.data(), v.size());
}
template <typename T, size_t N>
auto make_span(std::array<T, N>& v)
{
    return make_span(v.data(), v.size());
}

template <typename T, size_t N>
auto make_span(const T (&a)[N])
{
    return make_span(a, N);
}
template <typename T, size_t N>
auto make_span(T (&a)[N])
{
    return make_span(a, N);
}


}
#endif // CODA_OSS_sys_Span_h_INCLUDED_
