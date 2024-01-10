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

#include <type_traits>
#include <functional>
#include <cmath>

#include "Ximd_.h"
#include "ValArray_.h"

namespace sys
{
namespace ximd
{

template<typename T>
using Ximd = details::Ximd<T>;
//template<typename T>
//using Ximd = details::ValArray<T>;
//template<typename T>
//using Ximd = std::valarray<T>;

// template<typename T, int N>
// using fixed_size_ximd = Ximd<T, N>;
//
// template<typename T, int N>
// using native_ximd = Ximd<T>;

template <typename Mask>
inline bool any_of(const Mask& m)
{
    for (size_t i = 0; i < m.size(); i++)
    {
        if (m[i])
        {
            return true;
        }
    }
    return false;
}

template <typename T>
inline auto broadcast(T v)
{
    Ximd<T> result;
    details::broadcast(v, result);
    return result;
}

} // ximd
} // sys

#endif
