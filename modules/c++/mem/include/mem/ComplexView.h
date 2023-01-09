/* =========================================================================
 * This file is part of mem-c++
 * =========================================================================
 *
 * (C) Copyright 2013 - 2014, MDA Information Systems LLC
 * (C) Copyright 2023, Maxar Technologies, Inc.
 *
 * mem-c++ is free software; you can redistribute it and/or modify
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

#ifndef CODA_OSS_mem_ComplexView_h_INCLUDED_
#define CODA_OSS_mem_ComplexView_h_INCLUDED_
#pragma once

#include <cstddef>
#include <vector>
#include <memory>
#include <complex>

#include "coda_oss/span.h"

namespace mem
{
/*!
 *  \class ComplexView
 *  \brief This class provides a read-only view onto a collection of complex numbers.
 * For the simple case, it's std::span<const std::complex<T>>.  However, sometimes
 * the data is in two parallel arrays:
 *  const float* reals;
 *  const float* imags;
 * This class sets things up so that clients can always use the same
 * access routines, regardless of how the underlying data is actually stored.
 */
template <typename T>
struct ComplexView
{
    ComplexView() = default;
    ~ComplexView() = default;
    ComplexView(const ComplexView&) = default;
    ComplexView& operator=(const ComplexView&) = default;
    ComplexView(ComplexView&&) = default;
    ComplexView& operator=(ComplexView&&) = default;
};

#endif  // CODA_OSS_mem_ComplexView_h_INCLUDED_
