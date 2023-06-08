/* =========================================================================
 * This file is part of types-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2017, MDA Information Systems LLC
 *
 * types-c++ is free software; you can redistribute it and/or modify
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

#include "TestCase.h"

#include <types/complex_short.h>

TEST_CASE(TestCxShort_abs)
{
    constexpr auto real = 123;
    constexpr auto imag = -321;

    const std::complex<short> cx_short(real, imag);
    const auto expected = abs(cx_short);

    const types::complex_short types_cx_short(real, imag);
    const auto actual = abs(types_cx_short);
    TEST_ASSERT_EQ(actual, expected);
}

TEST_MAIN(
    TEST_CHECK(TestCxShort_abs);
    )
