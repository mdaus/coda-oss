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

template <typename TKey, typename TValue>
inline std::multimap<TValue, TKey> value_to_keys(const std::map<TKey, TValue>& key_to_value)
{
    std::multimap<TValue, TKey> retval;
    for (auto&& kv : key_to_value)
    {
        retval.emplace(std::make_pair(kv.second, kv.first));
    }
    return retval;
}

}  // namespace details

/**
 * Lookup the specified value in the map, returning the corresponding string if found.
 */
template <typename T>
inline coda_oss::optional<std::string> find_value(const T& v, const std::map<T, std::string>& value_to_string)
{
    return details::find(value_to_string, v);
}

/**
 * Lookup the specified value in the map, returning the corresponding strings if found.
 * 
 * This overload is intended to be used with the result of value_to_keys(); where map<std::string, T>
 * has multiple strings to the same value, e.g., "A" -> a and "a" -> a.
 */
template <typename T>
inline std::vector<std::string> find_value(const T& v, const std::multimap<T, std::string>& value_to_strings)
{
    return details::find(value_to_strings, v);
}

/**
 * Lookup the specified string in the map, returning the corresponding value if found.
 */
template <typename T>
inline coda_oss::optional<T> find_string(const std::string& s, const std::map<std::string, T>& string_to_value)
{
    return details::find(string_to_value, s);
}

// template <typename T>
//inline coda_oss::optional<std::string> toString(const T& v, std::nothrow_t)
//{
//     return find_value(v, coda_oss_enum_strings_to_values_(T()));
// }
// template <typename T>
// inline std::string toString(const T& v)
// {
//     return find_value(v, coda_oss_enum_strings_to_values_(T()));
// }

// template <typename T>
// inline coda_oss::optional<T> fromString(const std::string& v, std::nothrow_t)
//{
//     return find_string(v, coda_oss_enum_string_to_value_(T()));
// }
// template <typename T>
// inline T fromString(const std::string& v)
//{
//     return find_string(v, coda_oss_enum_string_to_value_(T()));
// }

}

#endif // CODA_OSS_enums_Convert_h_INCLUDED_
