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


#include <vector>
#include <string>
#include <map>
#include <new> // std::nothrow_t
#include <type_traits>
#include "coda_oss/optional.h"

// Utilities to convert enums to/from strings.

namespace enums
{
namespace details
{
template <typename T>
inline coda_oss::optional<T> make_optional(const std::vector<T>& values)
{
    return values.size() != 1 ? coda_oss::optional<T>() : coda_oss::optional<T>(values[0]);
}

/**
 * Lookup the specified key in the map, returning the corresponding value as an optional<>.
 */
template <typename TKey, typename TValue>
inline coda_oss::optional<TValue> contains(const std::map<TKey, TValue>& map, const TKey& key)
{
    // https://en.cppreference.com/w/cpp/container/map/contains
    const auto it = map.find(key);
    return it == map.end() ? coda_oss::optional<TValue>() : coda_oss::optional<TValue>(it->second);
}

/**
 * Lookup the specified key in the map, returning all the corresponding values.
 */
template <typename TKey, typename TValue>
inline std::vector<TValue> equal_range(const std::multimap<TKey, TValue>& map, const TKey& key)
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

// Extracts the value from the given optional<>, throwing the
// specified exception if has_value()==false.
template<typename T, typename TException>
inline const T& value(const coda_oss::optional<T>& v, const TException& ex)
{
    if (!v.has_value())
    {
        throw ex;
    }
    return *v;
}
template<typename T>
inline const T& value(const coda_oss::optional<T>& v)
{
    return value(v, std::invalid_argument("key not found."));
}

 template <typename T, typename TExceptionEmpty, typename TExceptionMultiple=TExceptionEmpty>
 inline const T& value(const std::vector<T>& values, const TExceptionEmpty& ex0, const TExceptionMultiple& ex2)
 {
     if (values.size() == 1)
     {
         return values[0];
     }
     if (values.empty())
     {
         throw ex0;
     }
     throw ex2;
 }
 template <typename T>
 inline const T& value(const std::vector<T>& values)
 {
     return value(values, std::invalid_argument("key not found."), std::invalid_argument("multiple keys found."));
 }

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

/**
 * Lookup the specified value in the map, returning the corresponding string if found.
 */
template <typename T>
inline coda_oss::optional<std::string> contains(const std::map<T, std::string>& value_to_string, const T& v)
{
    return details::contains(value_to_string, v);
}

/**
 * Lookup the specified value in the map, returning the corresponding strings if found.
 * 
 * This overload is intended to be used with the result of value_to_keys(); where map<std::string, T>
 * has multiple strings to the same value, e.g., "A" -> a and "a" -> a.
 */
template <typename T>
inline std::vector<std::string> find_value(const std::multimap<T, std::string>& value_to_strings, const T& v)
{
    return details::equal_range(value_to_strings, v);
}

/**
 * Lookup the specified string in the map, returning the corresponding value if found.
 */
template <typename T>
inline coda_oss::optional<T> contains(const std::map<std::string, T>& string_to_value, const std::string& s)
{
    return details::contains(string_to_value, s);
}

/**
 * Lookup the specified value in the map, return all the corresponding strings; see find_value()
 */
template <typename T>
inline std::vector<std::string> toStrings(const T& v)
{
    static const auto value_to_strings = details::value_to_keys(coda_oss_enums_string_to_value_(v));
    return find_value(value_to_strings, v);
}

/**
 * Lookup the specified value in the map, if there is just ONE corresponding string, return it;
 * otherwise NULL.  This routine provides no way to distinguish between ZERO return values
 * and MULTIPLE return values: both return NULL.
 */
 template <typename T>
inline coda_oss::optional<std::string> toString(const T& v, std::nothrow_t)
{
     const auto results = toStrings(v);
     return details::make_optional(results);
 }
 template <typename T, typename TExceptionEmpty, typename TExceptionMultiple>
 inline std::string toString(const T& v, const TExceptionEmpty& ex0, const TExceptionMultiple& ex2)
 {
     const auto results = toStrings(v);
     return details::value(results, ex0, ex2);
 }
  template <typename T, typename TException>
 inline std::string toString(const T& v, const TException& ex)
 {
     return toString(v, ex, ex);
 }
 template <typename T>
 inline std::string toString(const T& v)
 {
     const auto results = toStrings(v);
     return details::value(results);
 }

// It would be really nice to make these just `T fromString(const std::string&)` ...
// ... but I don't have enough C++ chops to do that.  The problem is that
// fromString() might need to be overloaded for a specific type (e.g., to 
// handle "OTHER*" in SIX), but that can't be done based on just the 
// return type.  And template specializations are messy because it all
// has to be in the same namespace.
//
// As it turns out, passing in an (unused) instance often results in
 // slightly shorter code as the type can be deduced.
/*
template <typename T>
inline coda_oss::optional<T> fromString(const std::string& s, std::nothrow_t)
{
    return find_string(s, coda_oss_enums_string_to_value_(T()));
}
template <typename T, typename TException>
inline T fromString(const std::string& s, const TException& ex)
{
    const auto result = fromString<T>(s, std::nothrow);
    return details::value(result, ex);
}
 template <typename T>
 inline T fromString(const std::string& s)
{
     const auto result = fromString<T>(s, std::nothrow);
     return details::value(result);
}
*/
template <typename T>
inline coda_oss::optional<T> fromString(const T& t, const std::string& s, std::nothrow_t)
{
    return contains(coda_oss_enums_string_to_value_(t), s);
}
template <typename T>
inline coda_oss::optional<T>& fromString(coda_oss::optional<T>& result, const std::string& s, std::nothrow_t)
{
    result = fromString(T(), s, std::nothrow);
    return result;
}

template <typename T, typename TException>
inline T fromString(const T& t, const std::string& s, const TException& ex)
{
    const auto result = fromString(t, s, std::nothrow);
    return details::value(result, ex);
}
template <typename T, typename TException>
inline T& fromString(T& result, const std::string& s, const TException& ex)
{
    result = fromString(static_cast<const T&>(result), s, ex);
    return result;
}

template <typename T>
inline T fromString(const T& t, const std::string& s)
{
    const auto result = fromString(t, s, std::nothrow);
    return details::value(result);
}
template <typename T>
inline T& fromString(T& result, const std::string& s)
{
    result = fromString(static_cast<const T&>(result), s);
    return result;
}

} // namespace enum

#endif // CODA_OSS_enums_Convert_h_INCLUDED_
