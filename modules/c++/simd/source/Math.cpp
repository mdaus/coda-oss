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

void simd::Sin(coda_oss::span<const float> inputs, coda_oss::span<float> outputs)
{
	// TODO: better implementation! This is just to get something going ...
    auto begin = inputs.begin();
    const auto end = inputs.end();
    auto it = outputs.begin();
    while (begin != end)
    {
        *it++ = sin(*begin++);
    }
}
void simd::Sin(coda_oss::span<const double> inputs, coda_oss::span<double> outputs)
{
	// TODO: better implementation! This is just to get something going ...
    auto begin = inputs.begin();
    const auto end = inputs.end();
    auto it = outputs.begin();
    while (begin != end)
    {
        *it++ = sin(*begin++);
    }
}