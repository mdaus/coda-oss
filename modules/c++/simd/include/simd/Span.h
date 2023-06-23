/* =========================================================================
 * This file is part of simd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * (C) Copyright 2023, Maxar Technologies, Inc.
 *
 * simd--c++ is free software; you can redistribute it and/or modify
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
#ifndef CODA_OSS_simd_Span_h_INCLUDED_
#define CODA_OSS_simd_Span_h_INCLUDED_

#include "coda_oss/span.h"

// This lets us use just `span` inside of the "simd" namepsace; see Math.h
namespace simd
{
	template<typename T>
	using span = coda_oss::span<T>;
}

#endif  // CODA_OSS_simd_Span_h_INCLUDED_
