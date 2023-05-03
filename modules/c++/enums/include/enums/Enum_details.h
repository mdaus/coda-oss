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

namespace enums
{
namespace details
{
// A key-value map could have multiple unique keys going to the same value, e.g,
// {{"+1", 1}, {"1", 1}}; i.e., both "+1" and "1" represent the integer value 1.
// "Reversing" that map requires a multimap, since for the same key `1` we
// have two values "+1 and "1".
template <typename TKey, typename TValue>
inline std::multimap<TValue, TKey> value_to_keys(
        const std::map<TKey, TValue>& key_to_value)
{
    std::multimap<TValue, TKey> retval;
    for (auto&& kv : key_to_value)
    {
        retval.emplace(std::make_pair(kv.second, kv.first));
    }
    return retval;
}

}  // namespace details
}  // namespace enum

#endif // CODA_OSS_enums_Enum_details_h_INCLUDED_
