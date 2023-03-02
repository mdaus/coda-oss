/* =========================================================================
 * This file is part of math.linear-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * math.linear-c++ is free software; you can redistribute it and/or modify
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
#ifndef CODA_OSS_math_linear_VectorMath_h_INCLUDED_
#define CODA_OSS_math_linear_VectorMath_h_INCLUDED_
#pragma once

#include <assert.h>

#include <cmath>
#include <memory> // std::nothrow_t

#include "coda_oss/span.h"

namespace math
{
namespace linear
{
 
/*!
    *  Compute the dot product between two vectors of equal size.  This method
    *  could just call the matrix multiply, though this method is more  efficient.
    * 
    *  \param vec Vector to dot with
    *  \return The magnitude
    */
template <typename T>
inline T dot(coda_oss::span<const T> lhs, coda_oss::span<const T> rhs, std::nothrow_t)
{
    T acc{0};
    assert(lhs.size() == rhs.size());
    for (size_t i = 0; i < lhs.size(); ++i)
    {
        acc += lhs[i] * rhs[i];
    }
    return acc;
}

template<typename T>
inline T dot(coda_oss::span<const T> lhs, coda_oss::span<const T> rhs)
{
    const auto sz = lhs.size();
    if (rhs.size() != sz)
        throw except::Exception(Ctxt("Dot product requires equal size vectors"));
    
    return dot(lhs, rhs, std::nothrow);
}

}
}

#endif  // CODA_OSS_math_linear_VectorMath_h_INCLUDED_
