/* =========================================================================
 * This file is part of math.linear-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * math.linear-c++ is free software; you can redistribute it and/or modify
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
#include <math/linear/Vector.h>
#include <math/linear/VectorN.h>
#include <math.h>

//Vector3D norm()--> normSq()
TEST_CASE("VaryingDimension")
{
    // Test case1: Control variable--components values
    //               Varying the dimensions
    math::linear::VectorN<1> v1(2.2); // normSq should be (2.2)^2. norm should be 2.2
    math::linear::VectorN<2> v2(2.2); // normSq should be 2 * (2.2)^2, norm should be sqrt of that
    math::linear::VectorN<3> v3(2.2); // normSq should be 3 * (2.2)^2, norm should be sqrt of that
    math::linear::VectorN<50> v4(2.2); // normSq should be 50 * (2.2)^2, norm should be sqrt of that

    // normSq:
    double expectedNormSq1 = std::pow(2.2, 2.0);
    double actualNormSq1 = v1.normSq();
    CHECK_THAT(expectedNormSq1, Catch::Matchers::WithinRel(actualNormSq1, 0.0001));

    double expectedNormSq2 = 2 * expectedNormSq1;
    double actualNormSq2 = v2.normSq();
    CHECK_THAT(expectedNormSq2, Catch::Matchers::WithinRel(actualNormSq2, 0.0001));

    double expectedNormSq3 = 3 * expectedNormSq1;
    double actualNormSq3 = v3.normSq();
    CHECK_THAT(expectedNormSq3, Catch::Matchers::WithinRel(actualNormSq3, 0.0001));

    double expectedNormSq4 = 50 * expectedNormSq1;
    double actualNormSq4 = v4.normSq();
    CHECK_THAT(expectedNormSq4, Catch::Matchers::WithinRel(actualNormSq4, 0.0001));

    // norm:
    double expectedNorm1 = std::sqrt(expectedNormSq1);
    double actualNorm1 = v1.norm();
    CHECK_THAT(expectedNorm1, Catch::Matchers::WithinRel(actualNorm1, 0.0001));

    double expectedNorm2 = std::sqrt(expectedNormSq2);
    double actualNorm2 = v2.norm();
    CHECK_THAT(expectedNorm2, Catch::Matchers::WithinRel(actualNorm2, 0.0001));

    double expectedNorm3 = std::sqrt(expectedNormSq3);
    double actualNorm3 = v3.norm();
    CHECK_THAT(expectedNorm3, Catch::Matchers::WithinRel(actualNorm3, 0.0001));

    double expectedNorm4 = std::sqrt(expectedNormSq4);
    double actualNorm4 = v4.norm();
    CHECK_THAT(expectedNorm4, Catch::Matchers::WithinRel(actualNorm4, 0.0001));
}
double manualNormSq(const math::linear::VectorN<3> v)
{
    return (v[0] * v[0]) + (v[1] * v[1]) + (v[2] * v[2]);
}
TEST_CASE("VaryingComponentVal")
{
    // Test case2: Control variable--dimension
    //             Varying each components value
    // normSq
    math::linear::VectorN<3> v5;
    v5[0] = 0; v5[1] = 1; v5[2] = 2; // normSq:
    double expectedNormSq5 = manualNormSq(v5);
    double actualNormSq5 = v5.normSq();
    CHECK_THAT(expectedNormSq5, Catch::Matchers::WithinRel(actualNormSq5, 0.0001));

    math::linear::VectorN<3> v6;
    v6[0] = -3; v6[1] = 39201; v6[2] = 0;
    double expectedNormSq6 = manualNormSq(v6);
    double actualNormSq6 = v6.normSq();
    CHECK_THAT(expectedNormSq6, Catch::Matchers::WithinRel(actualNormSq6, 0.0001));

    math::linear::VectorN<3> v7;
    v7[0] = -8; v7[1] = 54; v7[2] = 92;
    double expectedNormSq7 = manualNormSq(v7);
    double actualNormSq7 = v7.normSq();
    CHECK_THAT(expectedNormSq7, Catch::Matchers::WithinRel(actualNormSq7, 0.0001));

    math::linear::VectorN<3> v8;
    v8[0] = 0; v8[1] = 0; v8[2] = 0;
    double expectedNormSq8 = manualNormSq(v8);
    double actualNormSq8 = v8.normSq();
    CHECK_THAT(expectedNormSq8, Catch::Matchers::WithinRel(actualNormSq8, 0.0001));

    // norm
    double expectedNorm5 = std::sqrt(expectedNormSq5);
    double actualNorm5 = v5.norm();
    CHECK_THAT(expectedNorm5, Catch::Matchers::WithinRel(actualNorm5, 0.0001));

    double expectedNorm6 = std::sqrt(expectedNormSq6);
    double actualNorm6 = v6.norm();
    CHECK_THAT(expectedNorm6, Catch::Matchers::WithinRel(actualNorm6, 0.0001));

    double expectedNorm7 = std::sqrt(expectedNormSq7);
    double actualNorm7 = v7.norm();
    CHECK_THAT(expectedNorm7, Catch::Matchers::WithinRel(actualNorm7, 0.0001));

    double expectedNorm8 = std::sqrt(expectedNormSq8);
    double actualNorm8 = v8.norm();
    CHECK_THAT(expectedNorm8, Catch::Matchers::WithinRel(actualNorm8, 0.0001));
}
