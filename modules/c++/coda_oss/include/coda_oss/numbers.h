/* =========================================================================
 * This file is part of coda_oss-c++
 * =========================================================================
 *
 * (C) Copyright 2020-2022, Maxar Technologies, Inc.
 *
 * coda_oss-c++ is free software; you can redistribute it and/or modify
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
 * License along with this program; If not, http://www.gnu.org/licenses/.
 *
 */
#pragma once 
#ifndef CODA_OSS_coda_oss_numbers_h_INCLUDED_
#define CODA_OSS_coda_oss_numbers_h_INCLUDED_

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES // get M_PI, etc.
#endif
#include <math.h>

namespace coda_oss
{

namespace numbers
{
// https://en.cppreference.com/w/cpp/header/numbers

template <typename T>
constexpr T e_v = static_cast<T>(M_E);

template <typename T>
constexpr T pi_v = static_cast<T>(M_PI);

constexpr double e = e_v<double>;
constexpr double pi = pi_v<double>;

}

}

#endif  // CODA_OSS_coda_oss_numbers_h_INCLUDED_
