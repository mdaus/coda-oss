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

#include "enums/Enum.h"

#include "TestCase.h"

template<typename TEnum>
static void test_enums_Enum_(const std::string& testName, TEnum one, TEnum two, const std::string& strOne)
{
    TEnum v = one;
    TEST_ASSERT(v == one);
    TEST_ASSERT(v != two);

    auto i = static_cast<enums::underlying_type_t<TEnum>>(v);
    TEST_ASSERT_EQ(i, 1);

    v = static_cast<TEnum>(i);
    TEST_ASSERT(v == one);

    v = two;
    TEST_ASSERT(v == two);
    i = static_cast<enums::underlying_type_t<TEnum>>(v);
    TEST_ASSERT_EQ(i, 2);

    auto v2 = one;
    TEST_ASSERT(v2 == one);
    v2 = v;
    TEST_ASSERT(v2 == two);

    TEST_ASSERT(v == v2);
    TEST_ASSERT(!(v != v2));

    TEST_ASSERT(v == two);
    TEST_THROWS(enums::toString(v));  // no "Two"/"two" in map
    v = one;
    const auto result = enums::toString(v);
    TEST_ASSERT_EQ(result, strOne);

    static const TEnum T{};
    TEST_THROWS(enums::fromString(T, "abc"));
    TEST_THROWS(enums::fromString(T, "two"));  // no "Two"/"two" in map
    TEST_THROWS(enums::fromString(T, "Two"));  // no "Two"/"two" in map
    const auto v3 = enums::fromString(T, result);
    TEST_ASSERT(v3 == v);
    TEST_ASSERT(v3 == one);
}

TEST_CASE(test_enums_Enum_Number)
{
    using namespace enums::test;

    test_enums_Enum_<Numbers>(testName, Numbers::One, Numbers::Two, "One");

    Numbers v = Numbers::One;
    const auto i = static_cast<enums::underlying_type_t<Numbers>>(v);
    v = static_cast<Numbers>(i);
    v = Numbers::Two;
    auto v2 = Numbers::One;
    v2 = v;
    TEST_ASSERT(v == v2);
    TEST_ASSERT(!(v != v2));

    TEST_THROWS(enums::toString(v)); // no "Two" in map
    v = Numbers::One;
    const auto result = enums::toString(v);
    TEST_ASSERT_EQ(result, "One");
}

TEST_CASE(test_enums_Enum_number)
{
    using namespace enums::test;

    test_enums_Enum_<numbers>(testName, numbers::one, numbers::two, "one");

    numbers v = numbers::one;
    const auto i = static_cast<std::underlying_type<numbers>::type>(v);
    v = static_cast<numbers>(i);
    v = numbers::two;
    auto v2 = numbers::one;
    v2 = v;
    TEST_ASSERT(v == v2);
    TEST_ASSERT(!(v != v2));

    TEST_THROWS(enums::toString(v));  // no "two" in map
    v = numbers::one;
    const auto result = enums::toString(v);
    TEST_ASSERT_EQ(result, "one");
}

TEST_MAIN(
    TEST_CHECK(test_enums_Enum_Number);
    TEST_CHECK(test_enums_Enum_number);
    )
