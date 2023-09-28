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
#ifndef CODA_OSS_enums_Macros_h_INCLUDED_
#define CODA_OSS_enums_Macros_h_INCLUDED_

#include <ostream>

#include "enums/Convert.h"
#include "enums/Enum.h"

// Macros to generate "enum" code
#define CODA_OSS_enums_define_scoped_enum_(name, ...) enum class name { __VA_ARGS__ }

// Easily create map entries for the very common case where this is a one-to-one correspondence between v and "v".
#define CODA_OSS_enums_map_entry(name, n) { #n, name::n }

#define CODA_OSS_enums_map_entry_2(name, n1, n2) CODA_OSS_enums_map_entry(name, n1), CODA_OSS_enums_map_entry(name, n2)
#define CODA_OSS_enums_map_entry_3(name, n1, n2, n3) CODA_OSS_enums_map_entry(name, n1), CODA_OSS_enums_map_entry_2(name, n2, n3)
#define CODA_OSS_enums_map_entry_4(name, n1, n2, n3, n4)  CODA_OSS_enums_map_entry(name, n1), CODA_OSS_enums_map_entry_3(name, n2, n3, n4)
#define CODA_OSS_enums_map_entry_5(name, n1, n2, n3, n4, n5)  CODA_OSS_enums_map_entry(name, n1), CODA_OSS_enums_map_entry_4(name, n2, n3, n4, n5)

#define CODA_OSS_enums_define_ostream_op_insert(name) \
    inline std::ostream& operator<<(std::ostream& os, const name& v) { os << ::enums::toString(v); return os; }

#define CODA_OSS_enums_define_string_to_value_BEGIN(name)  CODA_OSS_enums_define_ostream_op_insert(name) \
   inline const std::map<std::string, name>& coda_oss_enums_string_to_value_(const name&) { static const std::map<std::string, name> retval {
#define CODA_OSS_enums_define_string_to_value_END }; return retval; }
#define CODA_OSS_enums_define_string_to_value_(name, ...) CODA_OSS_enums_define_string_to_value_BEGIN(name) __VA_ARGS__  \
    CODA_OSS_enums_define_string_to_value_END

#define CODA_OSS_enums_scoped_enum(n, name, ...) CODA_OSS_enums_define_scoped_enum_(name, __VA_ARGS__); \
        CODA_OSS_enums_define_string_to_value_(name, CODA_OSS_enums_map_entry_##n(name, __VA_ARGS__))


// And macros to generate "struct enum" (not C++11 "enum class") code; see Enum.h and Test.h

#define CODA_OSS_enums_define_struct_enum_values_(name, ...) namespace details { namespace Enum { \
    struct name { enum values { __VA_ARGS__ }; }; } }
#define CODA_OSS_enums_define_struct_enum_Enum_default_ctors_(name) name() = default; \
    name(const name&) = default; name(name&&) = default; \
    name& operator=(const name&) = default; name& operator=(name&&) = default
#define CODA_OSS_enums_define_struct_enum_Enum_ctors_(name) CODA_OSS_enums_define_struct_enum_Enum_default_ctors_(name); \
    name(value_t v) : Enum(v) {} explicit name(underlying_type_t i) : Enum(i) {}
#define CODA_OSS_enums_define_struct_enum_Enum_(name)  struct name final : public enums::Enum<details::Enum::name> { \
    CODA_OSS_enums_define_struct_enum_Enum_ctors_(name) }

#define CODA_OSS_enums_struct_enum(n, name, ...) CODA_OSS_enums_define_struct_enum_values_(name, __VA_ARGS__) \
    CODA_OSS_enums_define_struct_enum_Enum_(name); \
    CODA_OSS_enums_define_string_to_value_(name, CODA_OSS_enums_map_entry_##n(name, __VA_ARGS__))

#endif // CODA_OSS_enums_Macros_h_INCLUDED_
