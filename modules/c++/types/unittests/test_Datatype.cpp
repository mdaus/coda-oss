/* =========================================================================
 * This file is part of types-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2017, MDA Information Systems LLC
 *
 * types-c++ is free software; you can redistribute it and/or modify
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

#include "TestCase.h"

#include <types/Datatype.h>

struct Data final
{
    void* p;
    size_t count;
    size_t bytes;
    types::Datatype type;
};
template <typename T>
static Data new_Data(size_t cnt)
{
    constexpr auto type = types::getDatatype<T>();
    return Data{new T[cnt], cnt, cnt * sizeof(T), type};
}
template <typename T>
static inline void delete_Data_(Data& d)
{
    //const auto size_in_bytes = d.count * sizeof(T);
    //assert(d.bytes == size_in_bytes);
    delete[] static_cast<T*>(d.p);
}

static void delete_Data(Data& d)
{
    const auto f = [&](auto tag) {
        delete_Data_<types::Datatype_Tag<decltype(tag)>>(d);
    };
    types::invoke(d.type, f);  // delete_Data_<int>(d)
}
TEST_CASE(test_invoke_delete_Data)
{
    {
        auto data = new_Data<int>(10);
        TEST_ASSERT_NOT_NULL(data.p);
        TEST_ASSERT_EQ(data.count, 10);
        TEST_ASSERT_EQ(data.bytes, data.count * sizeof(int));
        TEST_ASSERT(data.type == types::Datatype::Int);
        delete_Data(data);
    }
    {
        auto data = new_Data<double>(100);
        TEST_ASSERT_NOT_NULL(data.p);
        TEST_ASSERT_EQ(data.count, 100);
        TEST_ASSERT_EQ(data.bytes, data.count * sizeof(double));
        TEST_ASSERT(data.type == types::Datatype::Double);
        delete_Data(data);
    }
}

static Data create_Data(types::Datatype type, size_t cnt)
{
    const auto f = [&](auto tag) {
        return new_Data<types::Datatype_Tag<decltype(tag)>>(cnt);
    };
    return types::invoke(type, f);  // new_Data<int>(cnt)
}
TEST_CASE(test_invoke_create_Data)
{
    {
        auto data = new_Data<int>(10);
        TEST_ASSERT_NOT_NULL(data.p);
        TEST_ASSERT_EQ(data.count, 10);
        TEST_ASSERT_EQ(data.bytes, data.count * sizeof(int));
        delete_Data(data);

        data = create_Data(types::Datatype::Int, 10);
        TEST_ASSERT_NOT_NULL(data.p);
        TEST_ASSERT_EQ(data.count, 10);
        TEST_ASSERT_EQ(data.bytes, data.count * sizeof(int));
        delete_Data(data);
    }
    {
        auto data = new_Data<double>(100);
        TEST_ASSERT_NOT_NULL(data.p);
        TEST_ASSERT_EQ(data.count, 100);
        TEST_ASSERT_EQ(data.bytes, data.count * sizeof(double));
        delete_Data(data);

        data = create_Data(types::Datatype::Double, 100);
        TEST_ASSERT_NOT_NULL(data.p);
        TEST_ASSERT_EQ(data.count, 100);
        TEST_ASSERT_EQ(data.bytes, data.count * sizeof(double));
        delete_Data(data);
    }
}

TEST_MAIN(
      TEST_CHECK(test_invoke_delete_Data);
      TEST_CHECK(test_invoke_create_Data);
    )
