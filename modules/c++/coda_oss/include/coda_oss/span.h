/* =========================================================================
 * This file is part of coda_oss-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * mem-c++ is free software; you can redistribute it and/or modify
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
#ifndef CODA_OSS_coda_oss_Span_h_INCLUDED_
#define CODA_OSS_coda_oss_Span_h_INCLUDED_
#pragma once

#include "gsl/gsl_span_.h"

namespace coda_oss
{
    template <typename T>
    using span = gsl::span<T>;

     template <typename T>
    inline span<T> make_span(T* d, size_t sz)
    {
        return gsl::make_span<T>(d, sz);
    }

     template <typename TContainer>
    inline span<typename TContainer::value_type> make_span(TContainer& c)
    {
        return gsl::make_span(c);
    }
    template <typename TContainer>
    inline span<typename TContainer::value_type> make_span(const TContainer& c)
    {
        return make_span(const_cast<TContainer&>(c));
    }

#define CODA_OSS_coda_oss_span 202002L  // c.f., __cpp_lib_span
}

#endif  // CODA_OSS_coda_oss_Span_h_INCLUDED_
