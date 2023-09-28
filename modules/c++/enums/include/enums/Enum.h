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

#pragma once
#ifndef CODA_OSS_enums_Enum_h_INCLUDED_
#define CODA_OSS_enums_Enum_h_INCLUDED_

#include <type_traits>
#include <stdexcept>
#include "coda_oss/optional.h"

#include "str/Convert.h"
#include "enums/Convert.h"

namespace enums
{
// There are scenarios that can't be done with the string<->value map.
// For example, mapping multiple strings, unknown at compile time, to
// enum values.  This happens in SIX where "OTHER_<something>"
// (read from a file) should be the enum value OTHER.
//
// Using a "struct enum" (not to be confused with "enum class"
// from C++11) allows an enum to hold arbitrary member-data
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
    Enum& operator=(value_t v) noexcept
    {
        value_ = v;
        return *this;
    }
    Enum(value_t v) noexcept
    {
        *this = v;
    }
    operator value_t() const noexcept
    {
        return value_;
    }

    // Be sure we're looking at a C-style "enum" not C++11 "enum class".
    using underlying_type_t_ = typename std::underlying_type<value_t>::type;
    using underlying_type_t = typename std::enable_if<std::is_enum<value_t>::value, underlying_type_t_>::type;
    underlying_type_t to_underlying() const noexcept
    {
        return static_cast<underlying_type_t>(value_);
    }
    // Explicit conversion to the underlying type; e.g., int
    explicit operator underlying_type_t() const noexcept
    {
        return to_underlying();
    }
    // allow `Enum e = static_cast<Enum>(i);` to work
    explicit Enum(underlying_type_t i) noexcept : Enum(static_cast<value_t>(i)) { }

private:
    value_t value_;
};

namespace details
{
    // Get the underlying type for a "struct Enum<T>" instance (above).
    // The std::enable_if<> gunk restricts this to Enum<T> classes
    template <typename TEnum>
    struct underlying_type<TEnum, typename std::enable_if<std::is_base_of<typename TEnum::struct_enum_t, TEnum>::value>::type>
    {
        using type = typename TEnum::underlying_type_t;
    }; // specialization for is_class<T>
} // namespace details

}

#endif // CODA_OSS_enums_Enum_h_INCLUDED_
