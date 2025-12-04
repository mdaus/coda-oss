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

#include <catch2/catch_test_macros.hpp>
#include <math/Utilities.h>

TEST_CASE("testNChooseK")
{
  CHECK(math::nChooseK(0, 0) == static_cast<size_t>(1));
  CHECK(math::nChooseK(1, 1) == static_cast<size_t>(1));
  CHECK(math::nChooseK(3, 2) == static_cast<size_t>(3));
  CHECK(math::nChooseK(10, 3) == static_cast<size_t>(120));
}

TEST_CASE("testNLessThanK")
{ 
    CHECK_THROWS(math::nChooseK(3, 10));
}
