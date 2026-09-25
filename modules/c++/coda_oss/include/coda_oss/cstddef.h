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
#include <gsl/byte>

namespace coda_oss
{
    // GSL byte will fall back to alias std::byte if available
	using gsl::byte;
}
static_assert(!std::is_same<coda_oss::byte, uint8_t>::value, "'coda_oss::byte' should be a unique type.");

#endif  // CODA_OSS_coda_oss_cstddef_h_INCLUDED_
