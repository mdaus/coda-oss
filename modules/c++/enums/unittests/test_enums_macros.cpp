/* =========================================================================
 * This file is part of enums-c++
 * =========================================================================
 *
 * (C) Copyright 2022, Maxar Technologies, Inc.
 *
 * enums-c++ is free software; you can redistribute it and/or modify
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
#include <std/string>
#include <std/optional>

#include "enums/Convert.h"
#include "enums/Test.h"

#include "TestCase.h"

TEST_CASE(test_enums_namespaces_toString)
{
    // Simple checks to ensure we generated enums correctly with the macros
    // and that everything works.

    auto result = enums::toString(enums::test::letters::x);
    TEST_ASSERT_EQ(result, "x");

    result = enums::toString(coda_oss::test_Letters::A);
    TEST_ASSERT_EQ(result, "A");
    result = enums::toString(coda_oss::test_letters::a);
    TEST_ASSERT_EQ(result, "a");

    result = enums::toString(coda_oss::test::letters::a);
    TEST_ASSERT_EQ(result, "a");
    result = enums::toString(coda_oss::enums::letters::a);
    TEST_ASSERT_EQ(result, "a");
    result = enums::toString(coda_oss::enums::test::letters::a);
    TEST_ASSERT_EQ(result, "a");
}

TEST_CASE(test_enums_namespaces_fromString)
{
    // Simple checks to ensure we generated enums correctly with the macros
    // and that everything works.
    {
        const auto result = enums::fromString(enums::test::letters(), "a");
        TEST_ASSERT(result == enums::test::letters::a);
    }

    // the enums generated with macros provide operator<<() so TEST_ASSERT_EQ works
    {
        const auto result = enums::fromString(coda_oss::test_Letters(), "A");
        TEST_ASSERT_EQ(result, coda_oss::test_Letters::A);
    }
    {
        const auto result = enums::fromString(coda_oss::test_letters (), "a");
        TEST_ASSERT_EQ(result, coda_oss::test_letters::a);
    }
    {
        const auto result = enums::fromString(coda_oss::test::letters(), "a");
        TEST_ASSERT_EQ(result, coda_oss::test::letters::a);
    }
    {
        const auto result = enums::fromString(coda_oss::enums::letters(), "a");
        TEST_ASSERT_EQ(result, coda_oss::enums::letters::a);
    }
    {
        const auto result = enums::fromString(coda_oss::enums::test::letters(), "a");
        TEST_ASSERT_EQ(result, coda_oss::enums::test::letters::a);
    }
}

TEST_MAIN(
    TEST_CHECK(test_enums_namespaces_toString);
    TEST_CHECK(test_enums_namespaces_fromString);
    )
