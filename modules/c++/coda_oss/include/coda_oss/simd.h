/* =========================================================================
 * This file is part of coda_oss-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * © Copyright 2024, Maxar Technologies, Inc.
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

#include "coda_oss/CPlusPlus.h"
#include "config/disable_compiler_warnings.h"

// This logic needs to be here rather than <std/simd> so that `coda_oss::simd` will
// be the same as `std::simd`.
#ifndef CODA_OSS_HAVE_std_simd_
    #define CODA_OSS_HAVE_std_simd_ 0  // assume no <simd>
#endif
#ifndef CODA_OSS_HAVE_experimental_simd_
    #define CODA_OSS_HAVE_experimental_simd_ 0  // assume no std::experimental::simd
#endif
#ifndef CODA_OSS_HAVE_vcl_simd_
    #define CODA_OSS_HAVE_vcl_simd_ 0  // assume no vcl::simd
#endif
#if CODA_OSS_cpp17 // __has_include
    #if __has_include(<simd>) // <simd> not until C++26 (hopefully!)
        #include <simd>
        #undef CODA_OSS_HAVE_std_simd_
        #define CODA_OSS_HAVE_std_simd_ 1  // provided by the implementation, probably C++26
    #endif
   
    #if __has_include(<experimental/simd>) // G++11
        #include <experimental/simd>
        #undef CODA_OSS_HAVE_experimental_simd_
        #define CODA_OSS_HAVE_experimental_simd_ 1  // provided by <experimental/simd>
    #endif

    #if __has_include("vectorclass/simd/simd")  // our own implementation using VCL
        #ifdef VCL_NAMESPACE
        #error "VCL_NAMESPACE already #define'd"
        #endif

        #define VCL_NAMESPACE vcl

        // The vectorclass headers #pragma-away some warnings; be sure those don't persist
        CODA_OSS_disable_warning_push
        #if _MSC_VER
        #pragma warning(disable: 4100) // '...': unreferenced formal parameter
        #pragma warning(disable: 4127) // conditional expression is constant
        #pragma warning(disable: 4244) // '...': conversion from '...' to '...', possible loss of data
        #pragma warning(disable: 4723) // potential divide by 0
        #pragma warning(disable: 6001) // Using uninitialized memory '...'.
        #pragma warning(disable: 26485) // Expression '...': No array to pointer decay (bounds.3).
        #pragma warning(disable: 26440) // Function '...' can be declared 'noexcept' (f.6).
        #pragma warning(disable: 26493) // Don't use C-style casts (type.4).
        #pragma warning(disable: 26814) // The const variable '...' can be computed at compile-time. Consider using constexpr (con.5).
        #pragma warning(disable: 26434) // Function '... hides a non-virtual function '...'.
        #pragma warning(disable: 26429) // Symbol '...' is never tested for nullness, it can be marked as not_null (f.23).
        #pragma warning(disable: 26482) // Only index into arrays using constant expressions (bounds.2).
        #pragma warning(disable: 26481) // Don't use pointer arithmetic. Use span instead (bounds.1).
        #pragma warning(disable: 26446) // Prefer to use gsl::at() instead of unchecked subscript operator (bounds.4).
        #pragma warning(disable: 26823) // Dereferencing a possibly null pointer '...' (lifetime.1).
        #pragma warning(disable: 26496) // The variable '...' does not change after construction, mark it as const (con.4).
        #pragma warning(disable: 26472) // Don't use a static_cast for arithmetic conversions. Use brace initialization, gsl::narrow_cast or gsl::narrow (type.1).
        #pragma warning(disable: 26494) // Variable '...' is uninitialized. Always initialize an object (type.5).
        #pragma warning(disable: 26458) // Prefer to use gsl::at() instead of unchecked subscript operator (bounds.4).
        #pragma warning(disable: 26497) // You can attempt to make '...' constexpr unless it contains any undefined behavior (f.4).
        #pragma warning(disable: 26818) // Switch statement does not cover all cases. Consider adding a '...' label (es.79).
        #pragma warning(disable: 26475) // Do not use function style casts (es.49). Prefer '...' over '...'.
        #pragma warning(disable: 26477) // Use '...' rather than 0 or NULL (es.47).

	#elif defined(__GNUC__) || defined(__clang__)

	CODA_OSS_disable_warning(-Wzero-as-null-pointer-constant)
	CODA_OSS_disable_warning(-Wshadow)

	#endif

        #include "vectorclass/version2/vectorclass.h"
        #include "vectorclass/version2/vectormath_trig.h"
        CODA_OSS_disable_warning_pop

        CODA_OSS_disable_warning_push
        #if _MSC_VER
        #pragma warning(disable: 26472) // Don't use a static_cast for arithmetic conversions. Use brace initialization, gsl::narrow_cast or gsl::narrow (type.1).
        #endif
        #include "vectorclass/simd/simd"
        CODA_OSS_disable_warning_pop

        #undef CODA_OSS_HAVE_vcl_simd_
        #define CODA_OSS_HAVE_vcl_simd_ 1  // provided by vectorclass/simd/simd.h
    #endif

#endif // CODA_OSS_cpp17

namespace coda_oss
{
    #if CODA_OSS_HAVE_std_simd_
        namespace simd = std::simd;
    #elif CODA_OSS_HAVE_experimental_simd_
        namespace simd = std::experimental;
    #elif CODA_OSS_HAVE_vcl_simd_
        namespace simd = vcl::simd;
    #endif 
}

