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

#include <vector>
#include <std/string>
#include <iterator>
#include <map>

#include "enums/Convert.h"
#include "enums/Test.h"

#include "TestCase.h"

TEST_CASE(test_enums_toString)
{
    //auto result = enums::toString(enums::test::letters::a);
    //TEST_ASSERT_EQ(result, "a");

    static const auto value_to_strings = enums::details::value_to_keys(coda_oss_enum_string_to_value_(enums::test::letters()));
    const auto results = enums::find_value(enums::test::letters::a, value_to_strings);
    TEST_ASSERT_EQ(results.size(), 2);
    for (auto&& r: results)
    {
        TEST_ASSERT((r == "a") || (r == "A"));    
    }
}

TEST_CASE(test_enums_fromString)
{
    //auto result = enums::fromString<enums::test::letters>("a");
    //TEST_ASSERT(result == enums::test::letters::a);
    //result = enums::fromString<enums::test::letters>("A");
    //TEST_ASSERT(result == enums::test::letters::a);
}

TEST_MAIN(
    TEST_CHECK(test_enums_toString);
    TEST_CHECK(test_enums_fromString);
    )
