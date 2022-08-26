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

#ifndef CODA_OSS_enums_Enum_h_INCLUDED_
#define CODA_OSS_enums_Enum_h_INCLUDED_
#pragma once

#include <type_traits>
#include "coda_oss/optional.h"

#include "enums/Convert.h"
#include "enums/Test.h"

namespace enums
{
// There are scenarios that can't be done with the string<->value map.
// For example, mapping multiple strings, unknown at compile time, to
// enum values.  This happens in SIX where "OTHER_<something>"
// (read from a file) should be the enum value OTHER.
//
// Using a "struct enum" (not to be confused with "enum class"
// from C++11 allows an enum to hold arbitrary member-data
// to support a situation like SIX.  It also supports legacy code
// that uses "struct enums" to simulate "enum class".

template <typename T, typename std::enable_if<std::is_class<T>::value, bool>::type = true>
using underlying_type_t = typename T::underlying_type_t;

template <typename T,  typename TClassValues,
    typename std::enable_if<std::is_class<TClassValues>::value, bool>::type = true>
struct Enum : public TClassValues
{
    using value_t = typename TClassValues::values;

    Enum() = default;
    Enum(const Enum&) = default;
    Enum(Enum&&) = default;
    Enum& operator=(const Enum&) = default;
    Enum& operator=(Enum&&) = default;

    // implicit conversion to the underlying enum
    Enum& operator=(value_t v)
    {
        value_ = v;
        return *this;
    }
    Enum(value_t v)
    {
        *this = v;
    }
    operator value_t() const
    {
        return value_;
    }

    // explicit conversion to the underlying type; e.g., int
    using underlying_type_t_ = typename std::underlying_type<value_t>::type;
    using underlying_type_t = typename std::enable_if<std::is_enum<value_t>::value, underlying_type_t_>::type;
    explicit operator underlying_type_t() const
    {
        return static_cast<underlying_type_t>(value_);
    }
    // allow `Enum e = static_cast<Enum>(i);` to work
    explicit Enum(underlying_type_t i) : Enum(static_cast<value_t>(i)) { }

private:
    value_t value_;
};

namespace test
{
    // "Spell out" everything (i.e., no macros) to make it "clear" (ha!) what's going on.
    namespace details { namespace Enum
    {
        struct Numbers
        {
            enum values { One, Two, Three };
        };
    } }
    struct Numbers final : public enums::Enum<Numbers, details::Enum::Numbers>
    {
        Numbers(value_t v) : Enum(v) {}
        explicit Numbers(underlying_type_t i) : Enum(i) {}
    };

    // `Numbers` (a "struct enum") and `numbers` (C++11 "enum class") should behave (about) the same.
    enum class numbers { one, two, three };
}

}

#endif // CODA_OSS_enums_Enum_h_INCLUDED_
