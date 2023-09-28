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
#ifndef CODA_OSS_enums_Enum_fromString_h_INCLUDED_
#define CODA_OSS_enums_Enum_fromString_h_INCLUDED_

#include <map>

#include "underlying_type.h"
#include "Enum_toString.h"

// Utilities to convert enums to/from strings.

namespace enums
{
    namespace details
    {
        template <typename TEnum>
        inline TEnum from_string(const std::string& s)
        {
        }
    }

    template<typename TEnum>
    inline TEnum from_string(const std::string& s)
    {

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

#endif // CODA_OSS_enums_Enum_fromString_h_INCLUDED_
