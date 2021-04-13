/* =========================================================================
 * This file is part of config-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
 *
 * config-c++ is free software; you can redistribute it and/or modify
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
#ifndef CODA_OSS_config_compiler_extentions_h_INCLUDED_
#define CODA_OSS_config_compiler_extentions_h_INCLUDED_
#pragma once

#include <config/coda_oss_config.h>

#ifndef CODA_OSS_attribute_noinline_DEFINED_
    #define CODA_OSS_attribute_noinline_DEFINED_ 1

    // https://stackoverflow.com/a/49468469/8877
    #if defined(__GNUC__)
        #define ATTRIBUTE_NOINLINE __attribute__((noinline))
        #define DECLSPEC_NOINLINE
        #define NOINLINE(RETURN_TYPE_) RETURN_TYPE_ ATTRIBUTE_NOINLINE
        #ifndef HAVE_ATTRIBUTE_NOINLINE
            #define HAVE_ATTRIBUTE_NOINLINE 1
        #endif
    #elif defined(_MSC_VER)
        #define ATTRIBUTE_NOINLINE
        #define DECLSPEC_NOINLINE __declspec(noinline)
        #define NOINLINE(RETURN_TYPE_) DECLSPEC_NOINLINE RETURN_TYPE_
        #ifndef HAVE_DECLSPEC_NOINLINE
            #define HAVE_DECLSPEC_NOINLINE 1
        #endif
    #else
        #define ATTRIBUTE_NOINLINE
        #define DECLSPEC_NOINLINE
        #define NOINLINE(RETURN_TYPE_) RETURN_TYPE_
    #endif
#endif // CODA_OSS_attribute_noinline_DEFINED_

#ifndef CODA_OSS_attribute_aligned_DEFINED_
    #define CODA_OSS_attribute_aligned_DEFINED_ 1

    #if defined(__GNUC__)
        // https://gcc.gnu.org/onlinedocs/gcc-3.2/gcc/Variable-Attributes.html
        #define ATTRIBUTE_ALIGNED(x) __attribute__((aligned(x)))
        #define DECLSPEC_ALIGN(x)
        #define ALIGNED(declaration, x) declaration ATTRIBUTE_ALIGNED(x)
        #ifndef HAVE_ATTRIBUTE_ALIGNED
            #define HAVE_ATTRIBUTE_ALIGNED 1
        #endif
    #elif defined(_MSC_VER)
        #define ATTRIBUTE_ALIGNED(x)
        #define DECLSPEC_ALIGN(x) __declspec(align(x))
        #define ALIGNED(declaration, x) DECLSPEC_ALIGN(x) declaration
        #ifndef HAVE_DECLSPEC_ALIGN
            #define HAVE_DECLSPEC_ALIGN 1
        #endif
    #else
        #define ATTRIBUTE_ALIGNED(X)
        #define DECLSPEC_ALIGN(x)
        #define ALIGNED(declaration, x) declaration
    #endif
#endif // CODA_OSS_attribute_aligned_DEFINED_

#ifdef HAVE_M256_DIRECT_INDEX
    #define _MM256_EXTRACTF(ymm_,i_) ymm_[i_]
#elif defined HAVE_M256_MEMBER_INDEX
    #define _MM256_EXTRACTF(ymm_,i_) ymm_.m256_f32[i_]
#else
    //This looks awful, but almost all of these intrinsics simply reinterpret bits and generate no actual instructions.
    #define _MM256_EXTRACTF(ymm_,i_) _mm256_cvtss_f32(_mm256_castsi256_ps(_mm256_set1_epi32(_mm256_extract_epi32(_mm256_castps_si256(ymm_),i_))))
#endif

#ifndef CODA_OSS_mm256_extractf_DEFINED_
    #define CODA_OSS_mm256_extractf_DEFINED_ 1

    #include <immintrin.h>
    namespace sys
    {
        template <typename T>
        inline T& mm256_extractf_(T& ymm, int i)
        {
            //This looks awful, but almost all of these intrinsics simply reinterpret bits and generate no actual instructions.
            #define CODA_OSS_sys_MM256_EXTRACTF_(ymm_,i_) _mm256_cvtss_f32(_mm256_castsi256_ps(_mm256_set1_epi32(_mm256_extract_epi32(_mm256_castps_si256(ymm_),i_))))
            return CODA_OSS_sys_MM256_EXTRACTF_(ymm, i);
        }
        template <typename T>
        inline float& mm256_extractf(T& ymm, int i)
        {
            #if defined(__GNUC__)
                return ymm[i];
            #elif defined(_MSC_VER)
                return ymm.m256_f32[i];
            #else
                return mm256_extractf_(ymm, i);
            #endif
        }
    }

#endif

#endif  // CODA_OSS_config_compiler_extentions_h_INCLUDED_
