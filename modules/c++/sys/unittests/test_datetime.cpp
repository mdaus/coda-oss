/* =========================================================================
 * This file is part of sys-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * sys-c++ is free software; you can redistribute it and/or modify
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

#include <sys/LocalDateTime.h>
#include <sys/UTCDateTime.h>
#include <sys/OS.h>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("testDefaultConstructor")
{
    sys::OS os;

    sys::LocalDateTime l1;
    sys::UTCDateTime u1;

    os.millisleep(100);

    sys::LocalDateTime l2;
    sys::UTCDateTime u2;

    CHECK(l1.getMonth() >= 1);
    CHECK(l1.getMonth() <= 12);
    CHECK(l1.getDayOfMonth() >= 1);
    CHECK(l1.getDayOfMonth() <= 31);
    CHECK(l1.getDayOfWeek() >= 1);
    CHECK(l1.getDayOfWeek() <= 7);
    CHECK(l1.getDayOfYear() >= 1);
    CHECK(l1.getDayOfYear() <= 366);
    CHECK(l1.getHour() >= 0);
    CHECK(l1.getHour() <= 23);
    CHECK(l1.getMinute() >= 0);
    CHECK(l1.getMinute() <= 59);
    CHECK(l1.getSecond() >= 0);
    CHECK(l1.getSecond() < 60);

    CHECK(u1.getMonth() >= 1);
    CHECK(u1.getMonth() <= 12);
    CHECK(u1.getDayOfMonth() >= 1);
    CHECK(u1.getDayOfMonth() <= 31);
    CHECK(u1.getDayOfWeek() >= 1);
    CHECK(u1.getDayOfWeek() <= 7);
    CHECK(u1.getDayOfYear() >= 1);
    CHECK(u1.getDayOfYear() <= 366);
    CHECK(u1.getHour() >= 0);
    CHECK(u1.getHour() <= 23);
    CHECK(u1.getMinute() >= 0);
    CHECK(u1.getMinute() <= 59);
    CHECK(u1.getSecond() >= 0);
    CHECK(u1.getSecond() < 60);

    CHECK(l2.getMonth() >= 1);
    CHECK(l2.getMonth() <= 12);
    CHECK(l2.getDayOfMonth() >= 1);
    CHECK(l2.getDayOfMonth() <= 31);
    CHECK(l2.getDayOfWeek() >= 1);
    CHECK(l2.getDayOfWeek() <= 7);
    CHECK(l2.getDayOfYear() >= 1);
    CHECK(l2.getDayOfYear() <= 366);
    CHECK(l2.getHour() >= 0);
    CHECK(l2.getHour() <= 23);
    CHECK(l2.getMinute() >= 0);
    CHECK(l2.getMinute() <= 59);
    CHECK(l2.getSecond() >= 0);
    CHECK(l2.getSecond() < 60);

    CHECK(u2.getMonth() >= 1);
    CHECK(u2.getMonth() <= 12);
    CHECK(u2.getDayOfMonth() >= 1);
    CHECK(u2.getDayOfMonth() <= 31);
    CHECK(u2.getDayOfWeek() >= 1);
    CHECK(u2.getDayOfWeek() <= 7);
    CHECK(u2.getDayOfYear() >= 1);
    CHECK(u2.getDayOfYear() <= 366);
    CHECK(u2.getHour() >= 0);
    CHECK(u2.getHour() <= 23);
    CHECK(u2.getMinute() >= 0);
    CHECK(u2.getMinute() <= 59);
    CHECK(u2.getSecond() >= 0);
    CHECK(u2.getSecond() < 60);

    CHECK(l1 == l1);
    CHECK(l1 != l2);
    CHECK(l1 < l2);
    CHECK(l2 > l1);
}

TEST_CASE("testParameterizedConstructor")
{
    sys::UTCDateTime u1(0);

    CHECK(u1.getYear() == 1970);
    CHECK(u1.getMonth() == 1);
    CHECK(u1.getHour() == 0);
    CHECK(u1.getMinute() == 0);
    CHECK(u1.getSecond() == 0);
    CHECK(u1.getTimeInMillis() == 0);

    // test H:M:S constructor
    sys::LocalDateTime l2(10, 2, 42.24);
    CHECK(l2.getHour() == 10);
    CHECK(l2.getMinute() == 2);
    CHECK(l2.getSecond() == 42.24);

    // test H:M:S constructor
    sys::UTCDateTime u2(10, 2, 42.24);
    CHECK(u2.getHour() == 10);
    CHECK(u2.getMinute() == 2);
    CHECK(u2.getSecond() == 42.24);

    // test YYYY:MM:DD constructor
    sys::LocalDateTime l3(2000, 1, 1);
    CHECK(l3.getYear() == 2000);
    CHECK(l3.getMonth() == 1);
    CHECK(l3.getDayOfMonth() == 1);
    CHECK(l3.getDayOfYear() == 1);

    // test YYYY:MM:DD constructor
    sys::UTCDateTime u3(2000, 1, 1);
    CHECK(u3.getYear() == 2000);
    CHECK(u3.getMonth() == 1);
    CHECK(u3.getDayOfMonth() == 1);
    CHECK(u3.getDayOfYear() == 1);

    // test YYYY:MM:DD:H:M:S constructor
    sys::LocalDateTime l4(2012, 9, 17, 8, 22, 43.0);
    CHECK(l4.getYear() == 2012);
    CHECK(l4.getMonth() == 9);
    CHECK(l4.getDayOfMonth() == 17);
    CHECK(l4.getDayOfYear() == 261);
    CHECK(l4.getHour() == 8);
    CHECK(l4.getMinute() == 22);
    CHECK(l4.getSecond() == 43.0);

    // test YYYY:MM:DD:H:M:S constructor
    sys::UTCDateTime u4(2012, 9, 17, 8, 22, 43.0);
    CHECK(u4.getYear() == 2012);
    CHECK(u4.getMonth() == 9);
    CHECK(u4.getDayOfMonth() == 17);
    CHECK(u4.getDayOfYear() == 261);
    CHECK(u4.getHour() == 8);
    CHECK(u4.getMinute() == 22);
    CHECK(u4.getSecond() == 43.0);

    // test string/format constructor
    sys::LocalDateTime l5(l4.format());
    CHECK(l5.getYear() == l4.getYear());
    CHECK(l5.getMonth() == l4.getMonth());
    CHECK(l5.getDayOfMonth() == l4.getDayOfMonth());
    CHECK(l5.getDayOfYear() == l4.getDayOfYear());
    CHECK(l5.getHour() == l4.getHour());
    CHECK(l5.getMinute() == l4.getMinute());
    CHECK(l5.getSecond() == l4.getSecond());

    // test string/format constructor
    sys::UTCDateTime u5(u4.format());
    CHECK(u5.getYear() == u4.getYear());
    CHECK(u5.getMonth() == u4.getMonth());
    CHECK(u5.getDayOfMonth() == u4.getDayOfMonth());
    CHECK(u5.getDayOfYear() == u4.getDayOfYear());
    CHECK(u5.getHour() == u4.getHour());
    CHECK(u5.getMinute() == u4.getMinute());
    CHECK(u5.getSecond() == u4.getSecond());
}

TEST_CASE("testDateTimeDetails")
{
    const time_t now = time(nullptr);
    {
        tm local;
        sys::DateTime::localtime(now, local);
        const auto ad = local.tm_year + 1900;  // "years since 1900"
        // this might break in 2038: https://en.wikipedia.org/wiki/Year_2038_problem
        CHECK(ad >= 1900);
        CHECK(ad >= 1970);
        CHECK(ad >= 2021);
        CHECK(local.tm_yday <= 365);  // "days since January 1"

        CHECK(ad == sys::LocalDateTime().getYear());
        CHECK(local.tm_yday + 1 == sys::LocalDateTime().getDayOfYear());
    }
    {
        tm global;
        sys::DateTime::gmtime(now, global);
        const auto ad = global.tm_year + 1900;  // "years since 1900"
        // this might break in 2038: https://en.wikipedia.org/wiki/Year_2038_problem
        CHECK(ad >= 1900);
        CHECK(ad >= 1970);
        CHECK(ad >= 2021);
        CHECK(global.tm_yday <= 365);  // "days since January 1"

        CHECK(ad == sys::UTCDateTime().getYear());
        CHECK(global.tm_yday + 1 == sys::UTCDateTime().getDayOfYear());
    }
}

TEST_CASE("testGetTimeInMillis")
{
    const sys::LocalDateTime lt;
    const auto result = lt.getTimeInMillis();
    CHECK(result >= 0.0);
    constexpr auto February_02_2021 = 1612928129.0 * 1000.0;  // in milliseconds
    CHECK(result >= February_02_2021);

    constexpr auto recent_past = February_02_2021 * 0.999;
    CHECK(result >= recent_past);

    constexpr auto far_into_the_future = February_02_2021 * 100.0;
    CHECK(result <= far_into_the_future);
}
