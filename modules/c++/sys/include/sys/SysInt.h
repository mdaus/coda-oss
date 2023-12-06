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

#pragma once
#ifndef CODA_OSS_sys_SysInt_h_INCLUDED_
#define CODA_OSS_sys_SysInt_h_INCLUDED_

#include <stdint.h>
#include <cstdint>

namespace sys
{
typedef char byte;
typedef unsigned char ubyte;
typedef uint8_t Uint8_T;
typedef uint16_t Uint16_T;
typedef uint32_t Uint32_T;
typedef uint64_t Uint64_T;
typedef size_t Size_T;
typedef int8_t Int8_T;
typedef int16_t Int16_T;
typedef int32_t Int32_T;
typedef int64_t Int64_T;

// From Herb Sutter's "cppfront" project:
// https://github.com/hsutter/cppfront/blob/main/include/cpp2util.h
//
//  Convenience names for fundamental types

//  Encouraged by default: Fixed-precision names
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

}

#endif // CODA_OSS_sys_SysInt_h_INCLUDED_
