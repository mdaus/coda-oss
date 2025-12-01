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
#include <units/Angles.h>
#include <units/Lengths.h>


TEST_CASE("test_degrees")
{
    constexpr units::Degrees<double> degrees_0 = 0;
    constexpr units::Degrees<double> degrees_90 = 90;
    constexpr units::Degrees<double> degrees_180 = 180;
    constexpr units::Degrees<double> degrees_270 = 270;
    constexpr units::Degrees<double> degrees_360 = 360;

    CHECK_THAT(0.0, Catch::Matchers::WithinAbs(sin(degrees_0), 0.0001));
    CHECK_THAT(1.0, Catch::Matchers::WithinAbs(sin(degrees_90), 0.0001));
    CHECK_THAT(0.0, Catch::Matchers::WithinAbs(sin(degrees_180), 0.0001));
    CHECK_THAT(-1.0, Catch::Matchers::WithinAbs(sin(degrees_270), 0.0001));
    CHECK_THAT(0.0, Catch::Matchers::WithinAbs(sin(degrees_360), 0.0001));

    CHECK_THAT(1.0, Catch::Matchers::WithinAbs(cos(degrees_0), 0.0001));
    CHECK_THAT(0.0, Catch::Matchers::WithinAbs(cos(degrees_90), 0.0001));
    CHECK_THAT(-1.0, Catch::Matchers::WithinAbs(cos(degrees_180), 0.0001));
    CHECK_THAT(0.0, Catch::Matchers::WithinAbs(cos(degrees_270), 0.0001));
    CHECK_THAT(1.0, Catch::Matchers::WithinAbs(cos(degrees_360), 0.0001));

    {
        using T = float;
        T sin, cos;
        const units::Degrees<T> deg_180 = 180;
        SinCos(deg_180, sin, cos);
        CHECK_THAT(static_cast<T>(0.0), Catch::Matchers::WithinAbs(sin, 0.0001f));
        CHECK_THAT(static_cast<T>(-1.0), Catch::Matchers::WithinAbs(cos, 0.0001f));
    }
    {
        using T = double;
        T sin, cos;
        const units::Degrees<T> deg_180 = 180;
        SinCos(deg_180, sin, cos);
        CHECK_THAT(static_cast<T>(0.0), Catch::Matchers::WithinAbs(sin, 0.0001));
        CHECK_THAT(static_cast<T>(-1.0), Catch::Matchers::WithinAbs(cos, 0.0001));
    }
    {
        using T = long double;
        T sin, cos;
        const units::Degrees<T> deg_180 = 180;
        SinCos(deg_180, sin, cos);
        CHECK_THAT(static_cast<T>(0.0), Catch::Matchers::WithinAbs(sin, 0.0001));
        CHECK_THAT(static_cast<T>(-1.0), Catch::Matchers::WithinAbs(cos, 0.0001));
    }
}

TEST_CASE("test_lengths")
{
    {
        constexpr units::Feet<double> feet_3 = 3;
        const auto same = feet_3.to();
        CHECK(same.value() == feet_3.value());

        units::Meters<double> meters{0};
        convert(feet_3, meters);  // convert ...
        CHECK_THAT(meters.value(), Catch::Matchers::WithinAbs(0.9144, std::numeric_limits<float>::epsilon()));
        const auto feet = meters.to<units::tags::Feet>();  // ...and back
        CHECK_THAT(feet.value(), Catch::Matchers::WithinAbs(feet_3.value(), std::numeric_limits<float>::epsilon()));
    }
    {
        constexpr auto meters_1 = units::make_Unit<units::tags::Meters>(1.0);
        const auto same = meters_1.to();
        CHECK(same.value() == meters_1.value());

        units::Feet<double> feet{0};
        convert(meters_1, feet);  // convert ...
        CHECK_THAT(feet.value(), Catch::Matchers::WithinAbs(3.2808398, std::numeric_limits<float>::epsilon()));
        const auto meters = feet.to<units::tags::Meters>();  // ...and back
        CHECK_THAT(meters.value(), Catch::Matchers::WithinAbs(meters_1.value(), std::numeric_limits<float>::epsilon()));
    }
}
