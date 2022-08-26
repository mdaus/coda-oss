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
static void test_enums_Enum_(const std::string& testName, TEnum one, TEnum two)
{
    TEnum v = one;
    //const auto i = static_cast<enums::underlying_type_t<TEnum>>(v);
    //v = static_cast<TEnum>(i);
    v = two;
    auto v2 = one;
    v2 = v;
    TEST_ASSERT(v == v2);
    TEST_ASSERT(!(v != v2));
}

TEST_CASE(test_enums_Enum_Number)
{
    using namespace enums::test;

    test_enums_Enum_<Numbers>(testName, Numbers::One, Numbers::Two);

    Numbers v = Numbers::One;
    const auto i = static_cast<enums::underlying_type_t<Numbers>>(v);
    v = static_cast<Numbers>(i);
    v = Numbers::Two;
    auto v2 = Numbers::One;
    v2 = v;
    TEST_ASSERT(v == v2);
    TEST_ASSERT(!(v != v2));
}

TEST_CASE(test_enums_Enum_number)
{
    using namespace enums::test;

    test_enums_Enum_<numbers>(testName, numbers::one, numbers::two);

    numbers v = numbers::one;
    const auto i = static_cast<std::underlying_type_t<numbers>>(v);
    v = static_cast<numbers>(i);
    v = numbers::two;
    auto v2 = numbers::one;
    v2 = v;
    TEST_ASSERT(v == v2);
    TEST_ASSERT(!(v != v2));
}

TEST_MAIN(
    TEST_CHECK(test_enums_Enum_Number);
    TEST_CHECK(test_enums_Enum_number);
    )
