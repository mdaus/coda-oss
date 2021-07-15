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

#include <TestCase.h>
#include <units/Angles.h>
#include <units/Lengths.h>

namespace
{
TEST_CASE(test_degrees)
{
    // These are part of "scene" not "six", but there aren't any unittests there
    const units::Degrees<double> degrees_0 = 0;
    const auto degrees_90 = units::asDegrees(90.0);
    const auto degrees_180 = units::asDegrees(180.0);
    const auto degrees_270 = units::asDegrees(270.0);
    const auto degrees_360 = units::asDegrees(360.0);

    TEST_ASSERT_ALMOST_EQ(0.0, sin(degrees_0));
    TEST_ASSERT_ALMOST_EQ(1.0, sin(degrees_90));
    TEST_ASSERT_ALMOST_EQ(0.0, sin(degrees_180));
    TEST_ASSERT_ALMOST_EQ(-1.0, sin(degrees_270));
    TEST_ASSERT_ALMOST_EQ(0.0, sin(degrees_360));

    TEST_ASSERT_ALMOST_EQ(1.0, cos(degrees_0));
    TEST_ASSERT_ALMOST_EQ(0.0, cos(degrees_90));
    TEST_ASSERT_ALMOST_EQ(-1.0, cos(degrees_180));
    TEST_ASSERT_ALMOST_EQ(0.0, cos(degrees_270));
    TEST_ASSERT_ALMOST_EQ(1.0, cos(degrees_360));
}

TEST_CASE(test_lengths)
{
    // These are part of "scene" not "six", but there aren't any unittests there
    const units::Feet<double> feet_3 = 3;
    const units::Meters<double> meters_1 = 1;

    TEST_ASSERT_ALMOST_EQ(toMeters(feet_3).value(), 0.9144);
    TEST_ASSERT_ALMOST_EQ(toFeet(meters_1).value(), 3.2808398);
}

}

int main(int /*argc*/, char** /*argv*/)
{
    TEST_CHECK(test_degrees);
    TEST_CHECK(test_lengths);
    return 0;
}

