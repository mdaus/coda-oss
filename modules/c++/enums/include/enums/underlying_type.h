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
#ifndef CODA_OSS_enums_underlying_type_h_INCLUDED_
#define CODA_OSS_enums_underlying_type_h_INCLUDED_

#include <type_traits>

namespace enums
{
namespace details
{
 // See example at https://en.cppreference.com/w/cpp/types/enable_if
 template <typename T, typename TEnable = void>
 struct underlying_type { };  // primary template

// Get the underlying type for a C++11 "enum class".
// is_scoped_enum<> is C++23 https://en.cppreference.com/w/cpp/types/is_scoped_enum
// so do !is_class<> instead. 
template <typename TScopedEnum>
struct underlying_type<TScopedEnum, typename std::enable_if_t<!std::is_class<TScopedEnum>::value>> // TODO: is_scoped_enum
{
    // https://en.cppreference.com/w/cpp/types/underlying_type
    using type = typename std::underlying_type_t<TScopedEnum>;
}; // specialization for is_scoped_enum<>

}  // namespace details

template <typename T>
using underlying_type_t = typename details::underlying_type<T>::type;

// https://en.cppreference.com/w/cpp/utility/to_underlying
template<typename T>
enums::underlying_type_t<T> to_underlying(T e) noexcept
{
    // "Equivalent to return static_cast<std::underlying_type_t<Enum>>(e);."
    return static_cast<enums::underlying_type_t<T>>(e);
}

} // namespace enum

#endif // CODA_OSS_enums_underlying_type_h_INCLUDED_
