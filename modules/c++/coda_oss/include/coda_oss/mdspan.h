/* =========================================================================
 * This file is part of coda_oss-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * (C) Copyright 2023, Maxar Technologies, Inc.
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
 * License along with this program; If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */
#pragma once
#ifndef CODA_OSS_coda_oss_mdspan_h_INCLUDED_
#define CODA_OSS_coda_oss_mdspan_h_INCLUDED_

#include "coda_oss/CPlusPlus.h"

// This should always work ... it's in a `details` namespace
#include "coda_oss/mdspan_.h"

// This logic needs to be here rather than <std/mdspan> so that `coda_oss::mdspan` will
// be the same as `std::mdspan`.
#ifndef CODA_OSS_HAVE_std_mdspan_
    #define CODA_OSS_HAVE_std_mdspan_ 0  // assume no <mdspan>
#endif
#if CODA_OSS_cpp17 // can't even #include this file with older C++14 compilers! :-(
    #if __has_include(<mdspan>) // __cpp_lib_mdspan not until C++23
        #include <mdspan>
        #undef CODA_OSS_HAVE_std_mdspan_
        #define CODA_OSS_HAVE_std_mdspan_ 1  // provided by the implementation, probably C++23
    #endif
    // Try the (much) more robust implemention from https://github.com/kokkos/mdspan
    #if __has_include("coda_oss/experimental/mdspan")
        #include "coda_oss/experimental/mdspan"
        #undef CODA_OSS_HAVE_std_mdspan_
        #define CODA_OSS_HAVE_std_mdspan_ 1  // provided coda_oss/experimental/mdspan
    #endif
#endif // CODA_OSS_cpp17

namespace coda_oss
{
    #if CODA_OSS_HAVE_std_mdspan_
        using std::experimental::mdspan;
        using std::experimental::dextents;
    #else
        using details::mdspan;
        using details::dextents;
    #endif 
}

#endif  // CODA_OSS_coda_oss_mdspan_h_INCLUDED_
