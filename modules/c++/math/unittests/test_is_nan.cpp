/* =========================================================================
 * This file is part of math-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2017, MDA Information Systems LLC
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

#include <limits.h>
#include <catch2/catch_test_macros.hpp>
#include <math/Utilities.h>

TEST_CASE("testIsNaNOnWeirdNumbers")
{
    CHECK(!math::isNaN(std::numeric_limits<double>::infinity()));
    CHECK(!math::isNaN(std::numeric_limits<double>::epsilon()));
    CHECK(!math::isNaN(std::numeric_limits<double>::max()));
    CHECK(!math::isNaN(std::numeric_limits<double>::min()));
}

TEST_CASE("testNaNs")
{
    CHECK(math::isNaN(std::numeric_limits<double>::quiet_NaN()));
    CHECK(math::isNaN(std::numeric_limits<double>::signaling_NaN()));
}

