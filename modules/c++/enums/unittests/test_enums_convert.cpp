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

TEST_CASE(test_enums_find_value)
{
    static const auto value_to_strings = enums::details::value_to_keys(coda_oss_enums_string_to_value_(enums::test::letters()));
    auto results = enums::find_value(enums::test::letters::a, value_to_strings);
    TEST_ASSERT_EQ(results.size(), 2);
    for (auto&& r: results)
    {
        TEST_ASSERT((r == "a") || (r == "A"));    
    }

    results = enums::find_value(enums::test::letters::q, value_to_strings);
    TEST_ASSERT_TRUE(results.empty()); // no "q" for q in map

    results = enums::find_value(enums::test::letters::x, value_to_strings);
    TEST_ASSERT_EQ(results.size(), 1);
    TEST_ASSERT_EQ(results[0], "x");
    TEST_ASSERT_NOT_EQ(results[0], "X");

    results = enums::find_value(enums::test::letters::y, value_to_strings);
    TEST_ASSERT_EQ(results.size(), 1);
    TEST_ASSERT_NOT_EQ(results[0], "y");
    TEST_ASSERT_EQ(results[0], "Y");

    results = enums::find_value(enums::test::letters::z, value_to_strings);
    TEST_ASSERT_EQ(results.size(), 1);
    TEST_ASSERT_NOT_EQ(results[0], "z");
    TEST_ASSERT_NOT_EQ(results[0], "Z");
    TEST_ASSERT_EQ(results[0], "zee");
}
TEST_CASE(test_enums_toStrings)
{
    auto results = enums::toStrings(enums::test::letters::a);
    TEST_ASSERT_EQ(results.size(), 2);
    for (auto&& r: results)
    {
        TEST_ASSERT((r == "a") || (r == "A"));    
    }

    results = enums::toStrings(enums::test::letters::q);
    TEST_ASSERT_TRUE(results.empty()); // no "q" for q in map

    results = enums::toStrings(enums::test::letters::x);
    TEST_ASSERT_EQ(results.size(), 1);
    TEST_ASSERT_EQ(results[0], "x");
    TEST_ASSERT_NOT_EQ(results[0], "X");

    results = enums::toStrings(enums::test::letters::y);
    TEST_ASSERT_EQ(results.size(), 1);
    TEST_ASSERT_NOT_EQ(results[0], "y");
    TEST_ASSERT_EQ(results[0], "Y");

    results = enums::toStrings(enums::test::letters::z);
    TEST_ASSERT_EQ(results.size(), 1);
    TEST_ASSERT_NOT_EQ(results[0], "z");
    TEST_ASSERT_NOT_EQ(results[0], "Z");
    TEST_ASSERT_EQ(results[0], "zee");
}
TEST_CASE(test_enums_toString_nothrow)
{
    {
        const auto result = enums::toString(enums::test::letters::a, std::nothrow);
        TEST_ASSERT_FALSE(result.has_value()); // entries for both "A" and "a"
    }
    {
        const auto result = enums::toString(enums::test::letters::q, std::nothrow);
        TEST_ASSERT_FALSE(result.has_value());  // no "q" for q in map
    }
    {
        const auto result = enums::toString(enums::test::letters::x, std::nothrow);
        TEST_ASSERT_TRUE(result.has_value());
        TEST_ASSERT_EQ(*result, "x");
    }
    {
        const auto result = enums::toString(enums::test::letters::y, std::nothrow);
        TEST_ASSERT_TRUE(result.has_value());
        TEST_ASSERT_EQ(*result, "Y");
    }
    {
        const auto result = enums::toString(enums::test::letters::z, std::nothrow);
        TEST_ASSERT_TRUE(result.has_value());
        TEST_ASSERT_EQ(*result, "zee");
    }
}
TEST_CASE(test_enums_toString_ex)
{
    const std::invalid_argument ex("toString() failed.");
    const std::invalid_argument ex0("key not found.");
    const std::invalid_argument ex2("multiple keys found.");

    TEST_THROWS(enums::toString(enums::test::letters::a, ex)); // entries for both "A" and "a"
    TEST_THROWS(enums::toString(enums::test::letters::a, ex0, ex2)); // entries for both "A" and "a"
    TEST_THROWS(enums::toString(enums::test::letters::q, ex));  // no "q" for q in map
    TEST_THROWS(enums::toString(enums::test::letters::q, ex0, ex2));  // no "q" for q in map

    auto result = enums::toString(enums::test::letters::x, ex);
    TEST_ASSERT_EQ(result, "x");
    result = enums::toString(enums::test::letters::x, ex0, ex2);
    TEST_ASSERT_EQ(result, "x");

    result = enums::toString(enums::test::letters::y, ex);
    TEST_ASSERT_EQ(result, "Y");
    result = enums::toString(enums::test::letters::y, ex0, ex2);
    TEST_ASSERT_EQ(result, "Y");

    result = enums::toString(enums::test::letters::z, ex);
    TEST_ASSERT_EQ(result, "zee");
    result = enums::toString(enums::test::letters::z, ex0, ex2);
    TEST_ASSERT_EQ(result, "zee");
}
TEST_CASE(test_enums_toString)
{
    TEST_THROWS(enums::toString(enums::test::letters::a)); // entries for both "A" and "a"
    TEST_THROWS(enums::toString(enums::test::letters::q));  // no "q" for q in map

    auto result = enums::toString(enums::test::letters::x);
    TEST_ASSERT_EQ(result, "x");

    result = enums::toString(enums::test::letters::y);
    TEST_ASSERT_EQ(result, "Y");

    result = enums::toString(enums::test::letters::z);
    TEST_ASSERT_EQ(result, "zee");
}

TEST_CASE(test_enums_find_string)
{
    static const auto& string_to_value = coda_oss_enums_string_to_value_(enums::test::letters());
    {
        const auto result = enums::find_string("a", string_to_value);
        TEST_ASSERT_TRUE(result.has_value());
        TEST_ASSERT(result == enums::test::letters::a);
        TEST_ASSERT(*result == enums::test::letters::a);
    }
    {
        const auto result = enums::find_string("A", string_to_value);
        TEST_ASSERT_TRUE(result.has_value());
        TEST_ASSERT(result == enums::test::letters::a);
        TEST_ASSERT(*result == enums::test::letters::a);
    }
    {
        const auto result = enums::find_string("q", string_to_value);
        TEST_ASSERT_FALSE(result.has_value());
    }
    {
        const auto result = enums::find_string("Q", string_to_value);
        TEST_ASSERT_FALSE(result.has_value());
    }
}

TEST_CASE(test_enums_fromString_nothrow)
{
    static const auto& string_to_value = coda_oss_enums_string_to_value_(enums::test::letters());
    static const enums::test::letters T;
    {
        const auto result = enums::fromString(T, "a", std::nothrow);
        TEST_ASSERT(result == enums::test::letters::a);
    }
    {
        const auto result = enums::fromString(T, "A", std::nothrow);
        TEST_ASSERT(result == enums::test::letters::a);
    }
    {
        const auto result = enums::fromString(T, "q", std::nothrow);
        TEST_ASSERT_FALSE(result.has_value());
    }
    {
        const auto result = enums::fromString(T, "Q", std::nothrow);
        TEST_ASSERT_FALSE(result.has_value());
    }
}

TEST_CASE(test_enums_fromString_ex)
{
    const std::invalid_argument ex("key not found.");

    static const auto& string_to_value = coda_oss_enums_string_to_value_(enums::test::letters());
    static const enums::test::letters T;
    {
        const auto result = enums::fromString(T, "a", ex);
        TEST_ASSERT(result == enums::test::letters::a);
    }
    {
        const auto result = enums::fromString(T, "A", ex);
        TEST_ASSERT(result == enums::test::letters::a);
    }
    TEST_THROWS(enums::fromString(T, "q", ex));
    TEST_THROWS(enums::fromString(T, "Q", ex));
}

TEST_CASE(test_enums_fromString)
{
    static const enums::test::letters T;

    auto result = enums::fromString(T, "a");
    TEST_ASSERT(result == enums::test::letters::a);
    result = enums::fromString(T, "A");
    TEST_ASSERT(result == enums::test::letters::a);

    TEST_THROWS(enums::fromString(T, "q"));
    TEST_THROWS(enums::fromString(T, "Q"));
}

TEST_MAIN(
    TEST_CHECK(test_enums_find_value);
    TEST_CHECK(test_enums_toStrings);
    TEST_CHECK(test_enums_toString_nothrow);
    TEST_CHECK(test_enums_toString_ex);
    TEST_CHECK(test_enums_toString);

    TEST_CHECK(test_enums_find_string);
    TEST_CHECK(test_enums_fromString_nothrow);
    TEST_CHECK(test_enums_fromString_ex);
    TEST_CHECK(test_enums_fromString);
    )
