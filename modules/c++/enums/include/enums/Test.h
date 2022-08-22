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

#ifndef CODA_OSS_enums_Test_h_INCLUDED_
#define CODA_OSS_enums_Test_h_INCLUDED_
#pragma once

#include <string>
#include <map>

// Some test frameworks don't place nice with "namespace" in the test code
namespace enums
{
namespace test
{
enum class letters
{
	a, b, c
};
inline std::map<std::string, letters> coda_oss_enum_string_to_value_(const letters&)
{
    static const std::map<std::string, letters> retval
    {
        {"a", letters::a}, {"A", letters::a}
        , { "b", letters::b }, { "B", letters::b }
        , { "c", letters::c }, { "C", letters::c }        
    };
    return retval;
}

} // namespace test
}

#endif // CODA_OSS_enums_Test_h_INCLUDED_
