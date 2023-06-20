/* =========================================================================
 * This file is part of simd--c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * (C) Copyright 2023, Maxar Technologies, Inc.
 *
 * simd--c++ is free software; you can redistribute it and/or modify
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

#include "simd/Math.h"

#include <iterator>

#include "simd/Vec.h"

void simd::Sin(coda_oss::span<const float> inputs, coda_oss::span<float> outputs)
{
    auto begin = inputs.begin();
    const auto end = inputs.end();
    auto it = outputs.begin();

    constexpr size_t width = 8;
    simd::Vec<float, width> vec;  // i.e., vcl::Vec4f

    while (begin != end)
    {
        // load_a() requires very strict alignment
        vec.load(&(*begin));

        const auto results = sin(vec);

        // ditto for store_a()
        results.store(&(*it));

        std::advance(begin, width);
        std::advance(it, width);
    }
}
void simd::Sin(coda_oss::span<const double> inputs, coda_oss::span<double> outputs)
{
    auto begin = inputs.begin();
    const auto end = inputs.end();
    auto it = outputs.begin();

    constexpr size_t width = 4;
    simd::Vec<double, width> vec; // i.e., vcl::Vec4d

    while (begin != end)
    {
        // load_a() requires very strict alignment
        vec.load(&(*begin));

        const auto results = sin(vec);

        // ditto for store_a()
        results.store(&(*it));

        std::advance(begin, width);
        std::advance(it, width);
    }
}