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
#include <map>
#include <string>

#include "enums/Enum_details.h"

#include "TestCase.h"

static const auto& key_to_value()
{
    static const std::map<std::string, int> retval = {{"0", 0}, {"+1", 1}, {"1", 1}};
    return retval;
}
static const auto& value_to_keys()
{
    static const auto retval = enums::details::value_to_keys(key_to_value());
    return retval;
}

TEST_CASE(test_enums_value_to_keys)
{
    const auto& result = value_to_keys();
    TEST_ASSERT_EQ(result.size(), 3);

    const auto it0 = result.find(0);
    TEST_ASSERT(it0 != result.end());
    TEST_ASSERT_EQ("0", it0->second);

    const auto it1 = result.find(1);
    TEST_ASSERT(it1 != result.end());
    size_t iter = 0;
    for (auto it = it1; it != result.end(); ++it)
    {
        TEST_ASSERT_EQ(it->first, 1);
        const auto str = it->second;
        TEST_ASSERT((str == "+1") || (str == "1"));
        iter++;
    }
    TEST_ASSERT_EQ(iter, 2);

     const auto it999 = result.find(999);
    TEST_ASSERT(it999 == result.end());
}

TEST_CASE(test_enums_find_key)
{
    const auto& m = key_to_value();

    const auto v_1 = enums::details::find(m, "+1");
    TEST_ASSERT_TRUE(v_1.has_value());
    TEST_ASSERT_EQ(*v_1, 1);
    const auto v1 = enums::details::find(m, "1");
    TEST_ASSERT_TRUE(v1.has_value());
    TEST_ASSERT_EQ(*v1, 1);

    const auto vXYZ = enums::details::find(m, "XYZ");
    TEST_ASSERT_FALSE(vXYZ.has_value());
}

TEST_CASE(test_enums_find_keys)
{
    const auto& mm = value_to_keys();

    auto results = enums::details::equal_range(mm, 0);
    TEST_ASSERT_EQ(results.size(), 1);

    results = enums::details::equal_range(mm, 1);
    TEST_ASSERT_EQ(results.size(), 2);
    for (auto&& v : results)
    {
        TEST_ASSERT((v == "+1") || (v == "1"));
    }

    results = enums::details::equal_range(mm, 999);
    TEST_ASSERT_TRUE(results.empty());
}

TEST_MAIN(
    TEST_CHECK(test_enums_value_to_keys);
    TEST_CHECK(test_enums_find_key);
    TEST_CHECK(test_enums_find_keys);
    )
