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
#ifndef CODA_OSS_enums_Test_h_INCLUDED_
#define CODA_OSS_enums_Test_h_INCLUDED_

#include <string>
#include <map>

#include "str/Manip.h"

#include "enums/Macros.h"
#include "enums/Enum.h"
#include "enums/Convert.h"

// Some test frameworks don't place nice with "namespace" in the test code
namespace enums
{
namespace test
{
enum class letters
{
    a, b, c, q, x, y, z
};
inline std::map<std::string, letters> coda_oss_enums_string_to_value_(const letters&)
{
    static const std::map<std::string, letters> retval
    {
        {"a", letters::a}, {"A", letters::a}
        , { "b", letters::b }, { "B", letters::b }
        , { "c", letters::c }, { "C", letters::c }
        // intentionally lacking "q" for testing

        , { "x", letters::x } // no upper-case
        , { "Y", letters::y} // no lower-case
        , {"zee", letters::z} // phonetic-spelling only
    };
    return retval;
}

} // namespace test
} // namespace enums

#include "enums/Macros.h"
// test other namespaces ... and macros
namespace coda_oss
{
    CODA_OSS_enums_scoped_enum(3, test_Letters, A, B, C);
    CODA_OSS_enums_scoped_enum(3, test_letters, a, b, c);
    namespace test
    {
        CODA_OSS_enums_scoped_enum(3, letters, a, b, c); // not to be confused with enums::test::letters
    }

    namespace enums // not to be confused with ::enums, this is coda_oss::enums
    {
        CODA_OSS_enums_scoped_enum(3, letters, a, b, c); // not to be confused with enums::test::letters
        namespace test
        {
            CODA_OSS_enums_scoped_enum(3, letters, a, b, c); // not to be confused with enums::test::letters
        }
    }
}

namespace enums
{
namespace test
{
    // "Spell out" everything (i.e., no macros) to make it "clear" (ha!) what's going on.
    namespace details { namespace Enum
    {
        struct Numbers
        {
            enum values { One = 1, Two, Three };
        };
    } }
    struct Numbers final : public enums::Enum<details::Enum::Numbers>
    {
        Numbers() = default;
        Numbers(const Numbers&) = default;
        Numbers(Numbers&&) = default;
        Numbers& operator=(const Numbers&) = default;
        Numbers& operator=(Numbers&&) = default;

        Numbers(value_t v) : Enum(v) {}
        explicit Numbers(underlying_type_t i) : Enum(i) {}
    };
    inline const std::map<std::string, Numbers>& coda_oss_enums_string_to_value_(const Numbers&) // see Convert.h for details
    {
        static const std::map<std::string, Numbers> retval
        {
                {"One", Numbers::One}
                // , {"Two", Numbers::Two}, // intentionally omitting for test purposes
                , {"Three", Numbers::Three}
        };
        return retval;
    }

    // `Numbers` (a "struct enum") and `numbers` (C++11 "enum class") should behave (about) the same.
    enum class numbers { one = 1, two, three };
    inline const std::map<std::string, numbers>& coda_oss_enums_string_to_value_(const numbers&)  // see Convert.h for details
    {
        static const std::map<std::string, numbers> retval
        {
                {"one", numbers::one}
                // , {"two", numbers::two}, // intentionally omitting for test purposes
                , {"three", numbers::three}
        };
        return retval;
    }

    // Copied from SIX: https://github.com/ngageoint/six-library/blob/master/six/modules/c%2B%2B/six/include/six/Enums.h
    namespace details { namespace Enum
    {
        struct PolarizationSequenceType 
        {
            enum values { OTHER, V, H, UNKNOWN }; // there are actually more values
        };
    } }
    struct PolarizationSequenceType final : public enums::Enum<details::Enum::PolarizationSequenceType>
    {
        PolarizationSequenceType() = default;
        PolarizationSequenceType(const PolarizationSequenceType&) = default;
        PolarizationSequenceType(PolarizationSequenceType&&) = default;
        PolarizationSequenceType& operator=(const PolarizationSequenceType&) = default;
        PolarizationSequenceType& operator=(PolarizationSequenceType&&) = default;

        PolarizationSequenceType(value_t v) : Enum(v) {}
        explicit PolarizationSequenceType(underlying_type_t i) : Enum(i) {}

        std::string other_;  // value of OTHER.* for SIDD 3.0/SICD 1.3
    };
    inline const std::map<std::string, PolarizationSequenceType>& coda_oss_enums_string_to_value_(const PolarizationSequenceType&) // see Convert.h for details
    {
        static const std::map<std::string, PolarizationSequenceType> retval
        {
                {"OTHER", PolarizationSequenceType::OTHER}
                , {"V", PolarizationSequenceType::V}
                , {"H", PolarizationSequenceType::H}
                , {"UNKNOWN", PolarizationSequenceType::UNKNOWN}
        };
        return retval;
    }
    inline std::string toString(const PolarizationSequenceType& v)
    {
        if ((v == PolarizationSequenceType::OTHER) && !v.other_.empty())
        {
            return v.other_;
        }
        return enums::toString(v);
    }
    inline PolarizationSequenceType fromString(const PolarizationSequenceType& t, const std::string& s)
    {
        if (str::starts_with(s, "OTHER_"))
        {
            PolarizationSequenceType retval = PolarizationSequenceType::OTHER;
            retval.other_ = s;
            return retval;
        }
        if (s == "UNKNOWN") // don't convert from "UNKNOWN" 
        {
            throw std::invalid_argument("'" + s + "' is invalid.");
        }
        return enums::fromString(t, s);
    }

    // Something simple, swiped from SIX again
    CODA_OSS_enums_struct_enum(5, OrientationType, UP, DOWN, LEFT, RIGHT, ARBITRARY);

} // namespace test

}

#endif // CODA_OSS_enums_Test_h_INCLUDED_
