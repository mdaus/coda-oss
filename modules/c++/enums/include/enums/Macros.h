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

#ifndef CODA_OSS_enums_Macros_h_INCLUDED_
#define CODA_OSS_enums_Macros_h_INCLUDED_
#pragma once

#include <ostream>

#include "enums/Convert.h"

// Macros to generate "enum" code
#define CODA_OSS_enums_define_scoped_enum_(name, ...) enum class name { __VA_ARGS__ }

// Easily create map entries for the very common case where this is a one-to-one correspondance between v and "v".
// The only catch is that slightly different syntax is needed for scoped enums ("enum class").
#define CODA_OSS_enums_map_entry_(name, n) { #n, n }
#define CODA_OSS_enums_scoped_map_entry_(name, n) { #n, name::n }
#define CODA_OSS_enums_emit_map_entry CODA_OSS_enums_scoped_map_entry_
#define CODA_OSS_enums_map_entry(name, n) CODA_OSS_enums_emit_map_entry(name, n)

#define CODA_OSS_enums_map_entry_2(name, n1, n2) CODA_OSS_enums_map_entry(name, n1), CODA_OSS_enums_map_entry(name, n2)
#define CODA_OSS_enums_map_entry_3(name, n1, n2, n3) CODA_OSS_enums_map_entry(name, n1), CODA_OSS_enums_map_entry_2(name, n2, n3)
#define CODA_OSS_enums_map_entry_4(name, n1, n2, n3, n4)  CODA_OSS_enums_map_entry(name, n1), CODA_OSS_enums_map_entry_3(name, n2, n3, n4)
#define CODA_OSS_enums_map_entry_5(name, n1, n2, n3, n4, n5)  CODA_OSS_enums_map_entry(name, n1), CODA_OSS_enums_map_entry_4(name, n2, n3, n4, n5)
#define CODA_OSS_enums_map_entry_6(name, n1, n2, n3, n4, n5, n6)  CODA_OSS_enums_map_entry(name, n1), CODA_OSS_enums_map_entry_5(name, n2, n3, n4, n5, n6)
#define CODA_OSS_enums_map_entry_7(name, n1, n2, n3, n4, n5, n6, n7) CODA_OSS_enums_map_entry(name, n1), CODA_OSS_enums_map_entry_6(name, n2, n3, n4, n5, n6, n7)
#define CODA_OSS_enums_map_entry_8(name, n1, n2, n3, n4, n5, n6, n7, n8) CODA_OSS_enums_map_entry(name, n1), CODA_OSS_enums_map_entry_7(name, n2, n3, n4, n5, n6, n7, n8)
#define CODA_OSS_enums_map_entry_9(name, n1, n2, n3, n4, n5, n6, n7, n8, n9) CODA_OSS_enums_map_entry(name, n1), CODA_OSS_enums_map_entry_8(name, n2, n3, n4, n5, n6, n7, n8, n9)
#define CODA_OSS_enums_map_entry_10(name, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10) CODA_OSS_enums_map_entry(name, n1), CODA_OSS_enums_map_entry_9(name, n2, n3, n4, n5, n6, n7, n8, n9, n10)
#define CODA_OSS_enums_map_entry_11(name, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11) CODA_OSS_enums_map_entry(name, n1), CODA_OSS_enums_map_entry_10(name, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11)
#define CODA_OSS_enums_map_entry_12(name, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12) CODA_OSS_enums_map_entry(name, n1), CODA_OSS_enums_map_entry_11(name, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12)
#define CODA_OSS_enums_map_entry_13(name, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13) CODA_OSS_enums_map_entry(name, n1), CODA_OSS_enums_map_entry_12(name, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13)
#define CODA_OSS_enums_map_entry_14(name, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13, n14) CODA_OSS_enums_map_entry(name, n1), CODA_OSS_enums_map_entry_13(name, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13, n14)
#define CODA_OSS_enums_map_entry_15(name, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13, n14, n15) CODA_OSS_enums_map_entry(name, n1), CODA_OSS_enums_map_entry_14(name, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13, n14, n15)


#define CODA_OSS_enums_define_ostream_op_insert_(name, toString) \
    inline std::ostream& operator<<(std::ostream& os, const name& v) { os << toString(v); return os; }
#define CODA_OSS_enums_define_ostream_toString(name) CODA_OSS_enums_define_ostream_op_insert_(name, ::enums::toString)

#define CODA_OSS_enums_define_string_to_value_begin(name)  CODA_OSS_enums_define_ostream_toString(name) \
   inline const std::map<std::string, name>& coda_oss_enums_string_to_value_(const name&) { static const std::map<std::string, name> retval {
#define CODA_OSS_enums_define_string_to_value_end }; return retval; }
#define CODA_OSS_enums_define_string_to_value_(name, ...) CODA_OSS_enums_define_string_to_value_begin(name) __VA_ARGS__  \
    CODA_OSS_enums_define_string_to_value_end

#define CODA_OSS_enums_scoped_enum(n, name, ...) CODA_OSS_enums_define_scoped_enum_(name, __VA_ARGS__); \
        CODA_OSS_enums_define_string_to_value_(name, CODA_OSS_enums_map_entry_##n(name, __VA_ARGS__))

#endif // CODA_OSS_enums_Macros_h_INCLUDED_
