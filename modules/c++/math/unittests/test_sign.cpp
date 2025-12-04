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
#include <math/Utilities.h>
#include <limits>
#include <numbers>

TEST_CASE("testZero")
{
   CHECK(math::sign(0) == 0);
   CHECK(math::sign(0.0) == 0);
}

TEST_CASE("testPositive")
{
    CHECK(math::sign(1) == 1);
    CHECK(math::sign(0.3) == 1);
    CHECK(math::sign(std::numeric_limits<double>::epsilon()) == 1);
}

TEST_CASE("testNegative")
{
    CHECK(math::sign(-std::numeric_limits<double>::epsilon()) == -1);
    CHECK(math::sign(-0.1) == -1);
}

TEST_CASE("testConstants")
{
    static auto pi = std::numbers::pi; // "Conditional expression is constant"
    CHECK(pi == M_PI);

    static auto e = std::numbers::e;  // "Conditional expression is constant"
    CHECK(e == M_E);
}
