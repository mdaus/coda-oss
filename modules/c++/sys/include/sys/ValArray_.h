/* =========================================================================
 * This file is part of sys-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * © Copyright 2024, Maxar Technologies, Inc.
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

#pragma once 

/*!
 *  \brief A simple (poor man's) "implemtation" of std::experimental::simd
 * for development/testing purposes.
 *
 * `std::experimental::simd` needs G++11 and C++20 and the "vectorclass"
 * library needs C++17.  "Rolling our own" using `std::array` lets developers
 * try out some things without needing actual SIMD code.
 *
 */

#include "Dbg.h"

// This is intended for development/testing purposes, so enable by default only for debug.
#ifndef CODA_OSS_Ximd_ENABLED
#define CODA_OSS_Ximd_ENABLED CODA_OSS_DEBUG
#endif

#if CODA_OSS_Ximd_ENABLED

#include <array>
#include <type_traits>
#include <functional>
#include <cmath>
#include <valarray>

namespace sys
{
namespace ximd
{
namespace details
{
// Need a class for the "broadcast" constructor (not impelemented).
template <typename T, int N = 4>
struct ValArray final : public std::valarray<T>
{
    using base_t = std::valarray<T>;
    ValArray(const base_t& other) : base_t(other)
    {
    }

    ValArray() : base_t(N)
    {
    }

    template <typename U>
    ValArray(U v) noexcept : ValArray(base_t(v, N))
    {
    }

    template <typename U>
    void copy_from(const U* mem)
    {
        *this = generate([&](size_t i) { return mem[i]; });
    }
    template <typename U>
    void copy_to(U* mem) const
    {
        for (size_t i = 0; i < size(); i++)
        {
            mem[i] = (*this)[i];
        }
    }

    // https://en.cppreference.com/w/cpp/experimental/simd/simd/simd
    // this is the same as `U&& v` above; avoid enable_if gunk for now.
    template <typename G>
    static auto generate(G&& generator) noexcept
    {
        ValArray retval;
        for (size_t i = 0; i < size(); i++)
        {
            retval[i] = generator(i);
        }
        return retval;
    }

    static constexpr size_t size() noexcept
    {
        return N;
    }

    ValArray& operator++() noexcept
    {
        *this = generate([&](size_t i) {
                    auto v = (*this)[i];
                    return ++v;
                });
        return *this;
    }
    ValArray operator++(int) noexcept
    {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }
};

template <typename T>
inline auto round(const ValArray<T>& v)
{
    return ValArray<T>::generate([&](size_t i) { return std::round(v[i]); });
}

} // details

} // ximd
} // sys

#endif
