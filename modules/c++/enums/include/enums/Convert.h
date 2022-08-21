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

#ifndef CODA_OSS_enums_Convert_h_INCLUDED_
#define CODA_OSS_enums_Convert_h_INCLUDED_
#pragma once

#include <string>
#include <map>
#include <vector>
#include "coda_oss/optional.h"

// Utilities to convert enums to/from strings.

namespace enums
{
namespace details
{
/**
 * Lookup the specified key in the map, returning the corresponding value as an optional<>.
 */
template <typename TKey, typename TValue>
inline coda_oss::optional<TValue> find(const std::map<TKey, TValue>& map, const TKey& key)
{
    const auto it = map.find(key);
    return it == map.end() ?
        coda_oss::optional<TValue>() : coda_oss::optional<TValue>(it->second);
}

/**
 * Lookup the specified key in the map, returning all the corresponding values.
 */
template <typename TKey, typename TValue>
inline std::vector<TValue> find(const std::multimap<TKey, TValue>& map, const TKey& key)
{
    std::vector<TValue> retval;

    // https://en.cppreference.com/w/cpp/container/multimap/equal_range
    const auto range = map.equal_range(key);
    for (auto it = range.first; it != range.second; ++it)
    {
        retval.push_back(it->second);
    }

    return retval;
}
}  // namespace details

/**
 * Lookup the specified value in the map, returning the corresponding string if found.
 */
template <typename T>
inline coda_oss::optional<std::string> toString(T v, const std::map<T, std::string>& values_to_strings)
{
    return find(values_to_string, v);
}

/**
 * Lookup the specified value in the map, returning the corresponding strings if found.
 */
template <typename T>
inline std::vector<std::string> toStrings(T v, const std::multimap<T, std::string>& values_to_strings)
{
    return find(values_to_strings, v);
}

/**
 * Lookup the specified string in the map, returning the corresponding value if found.
 */
template <typename T>
inline coda_oss::optional<T> fromString(const std::string& s, const std::map<std::string, T>& strings_to_values)
{
    return find(strings_to_values, s);
}

/**
 * Lookup the specified string in the map, returning all the corresponding values.
 */
template <typename T>
inline std::vector<T> fromString(const std::string& s, const std::multimap<std::string, T>& strings_to_values)
{
    return find(strings_to_values, s);
}

// template <typename T, typename TException>
// inline T value(const std::optional<T>& v, const TException& ex)
//{
//     if (!v.has_value())
//     {
//         throw ex;
//     }
//     return *v;
// }
// template <typename T>
// inline T value(const std::optional<T>& v)
//{
//     return value(v, std::invalid_argument("key not found in map."));
// }
// }
//
//     template <typename T>
// inline std::optional<std::string> to_string(
//         T v, const std::map<std::string, T>& string_to_enum, std::nothrow_t)
//{
//     static const auto enum_to_string =
//     details::swap_key_value(string_to_enum); return details::to_string(v,
//     enum_to_string);
// }
// template <typename T>
// inline std::string to_string(T v,
//                              const std::map<std::string, T>& string_to_enum)
//{
//     const auto result = to_string(v, string_to_enum, std::nothrow);
//     return details::value(result);
// }
//
// template <typename T>
// inline std::optional<T> from_string(
//         const std::string& v,
//         const std::map<std::string, T>& string_to_enum,
//         std::nothrow_t)
//{
//     return details::from_string(v, string_to_enum);
// }
// template <typename T>
// inline T from_string(const std::string& v,
//                      const std::map<std::string, T>& string_to_enum)
//{
//     const auto result = from_string(v, string_to_enum, std::nothrow);
//     return details::value(result);
// }

 template <typename T>
inline coda_oss::optional<std::string> toString(T v, std::nothrow_t)
{
     return toString(v, coda_oss_enum_strings_to_values_(T()));
 }
 template <typename T>
 inline std::string toString(T v)
 {
     return toString(v, coda_oss_enum_strings_to_values_(T()));
 }

 template <typename T>
 inline coda_oss::optional<T> fromString(const std::string& v, std::nothrow_t)
{
     return fromString(v, coda_oss_enum_strings_to_values_(T()));
 }
 template <typename T>
 inline T fromString(const std::string& v)
{
     return fromString(v, coda_oss_enum_strings_to_values_(T()));
 }
}
}

#endif // CODA_OSS_enums_Convert_h_INCLUDED_
