/* =========================================================================
 * This file is part of simd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2017, MDA Information Systems LLC
 *
 * simd-c++ is free software; you can redistribute it and/or modify
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

#include <vector>
#include <std/numbers>
#include <chrono>

#include <simd/Math.h>
#include <sys/Span.h>
#include <sys/Dbg.h>

#include "TestCase.h"

template<typename T>
static std::vector<T> make_values(size_t iterations)
{
    constexpr auto pi = std::numbers::pi;

    // https://en.cppreference.com/w/cpp/numeric/math/sin
    static const std::vector<T> angles{static_cast<T>(pi / 6), static_cast<T>(pi / 2), static_cast<T>(-3 * pi / 4),
        static_cast<T>(0.0), static_cast<T>(-0.0)};

    std::vector<T> retval;
    retval.reserve(angles.size() * iterations);
    for (size_t i = 0; i<iterations; i++)
    {
        retval.insert(retval.end(), angles.begin(), angles.end());
    }
    return retval;
}

template<typename T>
static void slow_Sin(coda_oss::span<const T> inputs, coda_oss::span<T> outputs)
{
    // This is intentionally slow so that we have something to compare with SIMD results
    auto begin = inputs.begin();
    const auto end = inputs.end();
    auto it = outputs.begin();
    while (begin != end)
    {
        *it++ = sin(*begin++);
    }
}

TEST_CASE(Test_simd_Sin)
{
    constexpr size_t iterations = sys::release ? 3000000 : 400;

    const auto inputs = make_values<float>(iterations);
    std::vector<float> results(inputs.size());

    simd::Sin(sys::make_span(inputs), sys::make_span(results));
    slow_Sin(sys::make_span(inputs), sys::make_span(results));

    simd::Sin(sys::make_span(inputs), sys::make_span(results));
    auto start = std::chrono::steady_clock::now();
    simd::Sin(sys::make_span(inputs), sys::make_span(results));
    auto end = std::chrono::steady_clock::now();
    TEST_ASSERT_ALMOST_EQ(results[0], 0.5f);
    TEST_ASSERT_ALMOST_EQ(results[1], 1.0f);
    TEST_ASSERT_ALMOST_EQ(results[2], -0.7071067812f);
    TEST_ASSERT_ALMOST_EQ(results[3], 0.0f);
    TEST_ASSERT_ALMOST_EQ(results[4], -0.0f);
    const std::chrono::duration<double> elapsed_simd = end - start;
    
    slow_Sin(sys::make_span(inputs), sys::make_span(results));
    start = std::chrono::steady_clock::now();
    slow_Sin(sys::make_span(inputs), sys::make_span(results));
    end = std::chrono::steady_clock::now();
    TEST_ASSERT_ALMOST_EQ(results[0], 0.5f);
    TEST_ASSERT_ALMOST_EQ(results[1], 1.0f);
    TEST_ASSERT_ALMOST_EQ(results[2], -0.7071067812f);
    TEST_ASSERT_ALMOST_EQ(results[3], 0.0f);
    TEST_ASSERT_ALMOST_EQ(results[4], -0.0f);
    const std::chrono::duration<double> elapsed_slow = end - start;

    #if NDEBUG // DEBUG SIMD code is slow
    const auto ratio = elapsed_slow / elapsed_simd;
    TEST_ASSERT_GREATER(ratio, 2.5);
    #endif
}

TEST_MAIN(

    TEST_CHECK(Test_simd_Sin);

)
