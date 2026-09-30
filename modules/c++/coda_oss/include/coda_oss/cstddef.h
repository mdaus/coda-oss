/* =========================================================================
 * This file is part of coda_oss-c++
 * =========================================================================
 *
 * (C) Copyright 2020, Maxar Technologies, Inc.
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
#ifndef CODA_OSS_coda_oss_cstddef_h_INCLUDED_
#define CODA_OSS_coda_oss_cstddef_h_INCLUDED_

#include <stdint.h>

#include <cstddef>
#include <type_traits>
#define _CODA_OSS_USE_GSL_BYTE 0

// Copy from GSL check to make sure we don't get all the deprecation warnings
#if ! (CODA_OSS_cpp17 && defined(__cpp_lib_byte))
    #include <gsl/byte>
    #undef _CODA_OSS_USE_GSL_BYTE
    #define _CODA_OSS_USE_GSL_BYTE 1
#endif

namespace coda_oss
{
#if _CODA_OSS_USE_GSL_BYTE
using gsl::byte;
#else
using std::byte;
#endif
}
static_assert(!std::is_same<coda_oss::byte, uint8_t>::value, "'coda_oss::byte' should be a unique type.");

#endif  // CODA_OSS_coda_oss_cstddef_h_INCLUDED_
