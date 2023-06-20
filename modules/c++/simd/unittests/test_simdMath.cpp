/* =========================================================================
 * This file is part of simd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2017, MDA Information Systems LLC
 *
 * simd-c++ is free software; you can redistribute it and/or modify
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

#include <vector>
#include <std/numbers>

#include <simd/Math.h>
#include <sys/Span.h>

#include "TestCase.h"

TEST_CASE(Test_simd_Sin)
{
    constexpr auto pi = std::numbers::pi;
    const std::vector<double> inputs{pi / 6, pi / 2, -3 * pi / 4, 0.0, -0.0};

    // https://en.cppreference.com/w/cpp/numeric/math/sin
    std::vector<double> results(inputs.size());
    simd::Sin(sys::make_span(inputs), sys::make_span(results));

    TEST_ASSERT_ALMOST_EQ(results[0], 0.5);
    TEST_ASSERT_ALMOST_EQ(results[1], 1.0);
    TEST_ASSERT_ALMOST_EQ(results[2], -0.7071067812);
    TEST_ASSERT_ALMOST_EQ(results[3], 0.0);
    TEST_ASSERT_ALMOST_EQ(results[4], -0.0);
}

TEST_MAIN(

    TEST_CHECK(Test_simd_Sin);

)
