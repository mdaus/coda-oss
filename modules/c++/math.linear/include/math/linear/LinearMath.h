/* =========================================================================
 * This file is part of math.linear-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * (C) Copyright 2023, Maxar Technologies, Inc.
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
#ifndef CODA_OSS_math_linear_LinearMath_h_INCLUDED_
#define CODA_OSS_math_linear_LinearMath_h_INCLUDED_
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

 /*
 * Find the square of the L2 norm
 * Sum of squares of the vector elements
 */
template <typename T>
inline T normSq(coda_oss::span<const T> m)
{
    const auto sz = m.size();
    T acc{0};
    for (size_t i = 0; i < sz; ++i)
    {
        acc += m[i] * m[i]; // this is likely faster than std::pow(m[i], 2) ???
    }
    return acc;
}

/*!
 *  Find the L2 norm of the matrix.
 *  \return The norm
 */
template <typename T>
T norm(coda_oss::span<const T> m)
{
    return ::sqrt(normSq(m));
}

//!  Compute normalized dot product 
template<typename T>
inline T normDot_(coda_oss::span<const T> lhs, coda_oss::span<const T> rhs, T dot)
{
    // We should be able to normalize the vectors first, then take the
    // dot product.  However, due numerical precision we could still end
    // up outsize the [-1, 1] range.

    const auto dotProduct = dot / (norm(lhs) * norm(rhs));
    return std::max<T>(std::min<T>(dotProduct, 1.0), -1.0);
}
template<typename T>
inline T normDot(coda_oss::span<const T> lhs, coda_oss::span<const T> rhs, std::nothrow_t)
{
    const auto dotProduct_ = dot(lhs, rhs, std::nothrow);
    return normDot_(lhs, rhs, dotProduct_);
}
template<typename T>
inline T normDot(coda_oss::span<const T> lhs, coda_oss::span<const T> rhs)
{
    const auto dotProduct_ = dot(lhs, rhs);
    return normDot_(lhs, rhs, dotProduct_);
}


}
}

#endif  // CODA_OSS_math_linear_LinearMath_h_INCLUDED_
