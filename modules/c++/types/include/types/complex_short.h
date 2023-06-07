/* =========================================================================
 * This file is part of types-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * (C) Copyright 2023, Maxar Technologies, Inc.
 *
 * types-c++ is free software; you can redistribute it and/or modify
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
#ifndef CODA_OSS_types_complex_short_h_INCLUDED_
#define CODA_OSS_types_complex_short_h_INCLUDED_

#include <complex>

#include "coda_oss/CPlusPlus.h"
#include "gsl/gsl.h"

namespace types
{
namespace details
{
/*!
 *  \class complex_short
 *  \brief std::complex<short>
 *
 * `std::complex<short>` is no longer valid C++; provide a (partial) work-around.
 * See https://en.cppreference.com/w/cpp/numeric/complex for detals.
 */
struct complex_short final
{
    using value_type = short;

    complex_short(value_type re = 0, value_type im = 0) : z{re, im} {}
    complex_short(const complex_short&) = default;
    complex_short& operator=(const complex_short&) = default;
    complex_short(complex_short&&) = default;
    complex_short& operator=(complex_short&&) = default;
    ~complex_short() = default;

    value_type real() const
    {
        return z[0];
    }
    void real(value_type value)
    {
        z[0] = value;
    }

    value_type imag() const
    {
        return z[1];
    }
    void imag(value_type value)
    {
        z[1] = value;
    }

private:
    value_type z[2]{0, 0};
};
static_assert(sizeof(complex_short) == sizeof(float), "sizeof(complex_short) != sizeof(float)");

// https://en.cppreference.com/w/cpp/numeric/complex/abs
inline auto abs(const complex_short& z)
{
    const std::complex<float> z_(z.real(), z.imag());
    const auto result = abs(z_);
    return gsl::narrow_cast<complex_short::value_type>(result);
}

}

#if CODA_OSS_cpp23
    using details::complex_short;
#else
    // no macro to turn this on/off, want to implement what we need in details::complex_short
    //using complex_short = std::complex<short>; // not valid in C++23
    using details::complex_short;

    static_assert(sizeof(std::complex<short>) == sizeof(complex_short), "sizeof(sizeof(std::complex<short>) != sizeof(complex_short)");
#endif
}

#endif  // CODA_OSS_types_complex_short_h_INCLUDED_
