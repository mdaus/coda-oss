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

#include "TestCase.h"

#include <simd/Vec.h>

TEST_CASE(Test_Vec_sin)
{
    simd::Vec<float, 4> f(1, 2, 3, 4);

}

TEST_MAIN(
    TEST_CHECK(Test_Vec_sin);
    )
