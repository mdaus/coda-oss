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

#include "enums/Test.h" // so we don't clobber other stuff

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

// Base class for a "struct enum."  The "trick" is to define the enum values
// in a separate struct first so that this class can derive from it.
// See example code below, although most clients will want to use the macros.
template <typename T,  typename std::enable_if<std::is_class<T>::value, bool>::type = true>
struct Enum : public T
{
    using struct_enum_t = T; // i.e., struct T { enum values { ... } } ;
    using value_t = typename struct_enum_t::values;

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

    // Be sure we're looking at a C-style "enum" not C++11 "enum class".
    using underlying_type_t_ = typename std::underlying_type<value_t>::type;
    using underlying_type_t = typename std::enable_if<std::is_enum<value_t>::value, underlying_type_t_>::type;
    // Explicit conversion to the underlying type; e.g., int
    explicit operator underlying_type_t() const
    {
        return static_cast<underlying_type_t>(value_);
    }
    // allow `Enum e = static_cast<Enum>(i);` to work
    explicit Enum(underlying_type_t i) : Enum(static_cast<value_t>(i)) { }

private:
    value_t value_;
};

namespace details
{
    // See example at https://en.cppreference.com/w/cpp/types/enable_if
    template <typename T, typename TEnable = void> struct underlying_type { };  // primary template

    // Get the underlying type for a "struct Enum<T>" instance (above).
    // The std::enable_if<> gunk restricts this to Enum<T> classes
    template <typename TEnum>
    struct underlying_type<TEnum, typename std::enable_if<std::is_base_of<typename TEnum::struct_enum_t, TEnum>::value>::type>
    {
        using type = typename TEnum::underlying_type_t;
    }; // specialization for is_class<T>

    // Get the underlying type for a C++11 "enum class".
    // is_scoped_enum<> is C++23 https://en.cppreference.com/w/cpp/types/is_scoped_enum
    // so do !is_class<> instead. 
    template <typename TScopedEnum>
    struct underlying_type<TScopedEnum, typename std::enable_if<!std::is_class<TScopedEnum>::value>::type> // TODO: is_scoped_enum
    {
        // https://en.cppreference.com/w/cpp/types/underlying_type
        using type = typename std::underlying_type<TScopedEnum>::type;
    }; // specialization for is_scoped_enum<>
}
template <typename T>
using underlying_type_t = typename details::underlying_type<T>::type;

namespace test
{
    // "Spell out" everything (i.e., no macros) to make it "clear" (ha!) what's going on.
    namespace details { namespace Enum
    {
        struct Numbers
        {
            enum values { Zero, One, Two, Three };
        };
    } }
    struct Numbers final : public enums::Enum<details::Enum::Numbers>
    {
        Numbers() = default;
        Numbers(const Numbers&) = default;
        Numbers(Numbers&&) = default;
        Numbers& operator=(const Numbers&) = default;
        Numbers& operator=(Numbers&&) = default;

        Numbers(value_t v) : Enum(v) {}
        explicit Numbers(underlying_type_t i) : Enum(i) {}
    };
    inline std::map<std::string, Numbers> coda_oss_enums_string_to_value_(const Numbers&) // see Convert.h for details
    {
        static const std::map<std::string, Numbers> retval
        {
                {"Zero", Numbers::Zero}
                , {"One", Numbers::One}
                // , {"Two", Numbers::Two}, // intentionlly omitting for test purposes
                , {"Three", Numbers::Three}
        };
        return retval;
    }

    // `Numbers` (a "struct enum") and `numbers` (C++11 "enum class") should behave (about) the same.
    enum class numbers { zero, one, two, three };
    inline std::map<std::string, numbers> coda_oss_enums_string_to_value_(const numbers&)  // see Convert.h for details
    {
        static const std::map<std::string, numbers> retval
        {
                {"zero", numbers::zero},
                {"one", numbers::one}
                // , {"two", numbers::two}, // intentionlly omitting for test purposes
                , {"three", numbers::three}
        };
        return retval;
    }
}

}

#endif // CODA_OSS_enums_Enum_h_INCLUDED_
