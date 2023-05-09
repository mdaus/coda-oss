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

#ifndef CODA_OSS_enums_Enum_details_h_INCLUDED_
#define CODA_OSS_enums_Enum_details_h_INCLUDED_
#pragma once

#include <map>
#include <vector>
#include <new> // std::nothrow_t
#include <stdexcept>
#include "coda_oss/optional.h"

namespace enums
{
namespace details
{
/**
 * A key-value map could have multiple unique keys going to the same value, e.g,
 * {{"+1", 1}, {"1", 1}}; i.e., both "+1" and "1" represent the integer value 1.
 * "Reversing" that map requires a multimap, since for the same key `1` we
 * have two values "+1 and "1".
 */
template <typename TKey, typename TValue>
inline auto value_to_keys(const std::map<TKey, TValue>& key_to_value)
{
    std::multimap<TValue, TKey> retval;
    for (auto&& kv : key_to_value)
    {
        retval.emplace(std::make_pair(kv.second, kv.first));
    }
    return retval;
}

/**
 * Lookup the specified key in the map; return NULL if not found.
 */
template <typename TKey, typename TValue, typename TKeyArg = TKey>
inline auto find(const std::map<TKey, TValue>& m, const TKeyArg& key, std::nothrow_t)
{
    const auto it = m.find(key);
    return it == m.end() ? coda_oss::optional<TValue>() : coda_oss::optional<TValue>(it->second);
}
template <typename TKey, typename TValue, typename TException, typename TKeyArg = TKey>
inline auto find(const std::map<TKey, TValue>& m, const TKeyArg& key, const TException& ex)
{
    const auto v = find(m, key, std::nothrow);
    if (v.has_value())
    {
        return v.value();
    }
    throw ex;
}
template <typename TKey, typename TValue, typename TKeyArg = TKey>
inline auto find(const std::map<TKey, TValue>& m, const TKeyArg& key)
{
    return find(m, key, std::invalid_argument("key"));
}

/**
 * Lookup the specified key in the multimap; return all matching values or an empty vector if not found.
 */
template <typename TKey, typename TValue, typename TKeyArg = TKey>
inline auto equal_range(const std::multimap<TKey, TValue>& mm, const TKeyArg& key, std::nothrow_t)
{
    std::vector<TValue> retval;

    // https://en.cppreference.com/w/cpp/container/multimap/equal_range
    const auto range = mm.equal_range(key);
    for (auto it = range.first; it != range.second; ++it)
    {
        retval.push_back(it->second);
    }

    return retval;
}
template <typename TKey, typename TValue,  typename TException, typename TKeyArg = TKey>
inline auto equal_range(const std::multimap<TKey, TValue>& mm, const TKeyArg& key, const TException& ex)
{
    auto retval = equal_range(mm, key, std::nothrow);
    if (!retval.empty())
    {
        return retval;
    }
    throw ex;
}
template <typename TKey, typename TValue,  typename TKeyArg = TKey>
inline auto equal_range(const std::multimap<TKey, TValue>& mm, const TKeyArg& key)
{
    return equal_range(mm, key, std::invalid_argument("key"));
}

}  // namespace details
}  // namespace enum

#endif // CODA_OSS_enums_Enum_details_h_INCLUDED_
