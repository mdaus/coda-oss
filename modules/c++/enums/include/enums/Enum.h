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


#include <vector>
#include <string>
#include <map>
#include <new> // std::nothrow_t
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

// First, "spell out" everything by hand for illustration purposes
namespace test
{
namespace details { namespace Enum { namespace values {
struct numbers
{
    enum values { one, two, three };
}; 
} } }
struct numbers final : public details::Enum::values::numbers
{
};
inline std::map<std::string, numbers> coda_oss_enums_string_to_value_(const numbers&)
{
    static const std::map<std::string, numbers> retval
    {
        {"one", numbers::one }, 
        //, { "two", numbers::two } // intentionally leaving out for testing purposes
        , { "three", numbers::three }
    };
    return retval;
}
} // namespace test

}

#endif // CODA_OSS_enums_Enum_h_INCLUDED_
