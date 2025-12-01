/* =========================================================================
 * This file is part of math-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2016, MDA Information Systems LLC
 *
 * math-c++ is free software; you can redistribute it and/or modify
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

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <math/Bessel.h>

constexpr double eps = static_cast<double>(std::numeric_limits<float>::epsilon());

TEST_CASE("orderZero")
{
    CHECK_THAT(math::besselI(0, 1), Catch::Matchers::WithinRel(1.266065878, eps));
    CHECK_THAT(math::besselI(0, 2), Catch::Matchers::WithinRel(2.279585302, eps));
    CHECK_THAT(math::besselI(0, 3), Catch::Matchers::WithinRel(4.88079259, eps));
}

TEST_CASE("orderOne")
{
    CHECK_THAT(math::besselI(1, 4), Catch::Matchers::WithinRel(9.759465154, eps));
    CHECK_THAT(math::besselI(1, 5), Catch::Matchers::WithinRel(24.33564214, 1e-5));
    CHECK_THAT(math::besselI(1, 5.6), Catch::Matchers::WithinRel(42.32828803, 1e-5));
}

TEST_CASE("orderFive")
{
    CHECK_THAT(math::besselI(5, -1), Catch::Matchers::WithinRel(-2.71463156e-4, eps));
    CHECK_THAT(math::besselI(5, 0), Catch::Matchers::WithinRel(0, eps));
    CHECK_THAT(math::besselI(5, 1), Catch::Matchers::WithinRel(2.71463156e-4, eps));
}
