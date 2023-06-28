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
#include <std/span>

#include <simd/Math.h>
#include <sys/Span.h>
#include <sys/Dbg.h>
#include <sys/OS.h>

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
template <typename T>
static void slow_Cos(coda_oss::span<const T> inputs, coda_oss::span<T> outputs)
{
    // This is intentionally slow so that we have something to compare with SIMD
    // results
    auto begin = inputs.begin();
    const auto end = inputs.end();
    auto it = outputs.begin();
    while (begin != end)
    {
        *it++ = cos(*begin++);
    }
}

static void test_simd_Sin_almost_equal(const std::string& testName, const float* pResults, const size_t size = 5)
{
    if (size >= 1) TEST_ASSERT_ALMOST_EQ(pResults[0], 0.5f);
    if (size >= 2) TEST_ASSERT_ALMOST_EQ(pResults[1], 1.0f);
    if (size >= 3) TEST_ASSERT_ALMOST_EQ(pResults[2], -0.7071067812f);
    if (size >= 4) TEST_ASSERT_ALMOST_EQ(pResults[3], 0.0f);
    if (size >= 5) TEST_ASSERT_ALMOST_EQ(pResults[4], -0.0f);
}
TEST_CASE(Test_simd_Sin)
{
    constexpr size_t iterations = sys::release ? 4000000 : 400;

    const auto inputs = make_values<float>(iterations);
    std::vector<float> results(inputs.size());

    simd::Sin(sys::make_span(inputs), sys::make_span(results));
    auto start = std::chrono::steady_clock::now();
    simd::Sin(sys::make_span(inputs), sys::make_span(results));
    auto end = std::chrono::steady_clock::now();
    test_simd_Sin_almost_equal(testName, &(results[0]));
    const std::chrono::duration<double> elapsed_simd = end - start;
    
    slow_Sin(sys::make_span(inputs), sys::make_span(results));
    start = std::chrono::steady_clock::now();
    slow_Sin(sys::make_span(inputs), sys::make_span(results));
    end = std::chrono::steady_clock::now();
    test_simd_Sin_almost_equal(testName, &(results[0]));
    const std::chrono::duration<double> elapsed_slow = end - start;

    #if NDEBUG // DEBUG SIMD code is slow
    const auto ratio = elapsed_slow / elapsed_simd;
    // Ratios observed by testing
    constexpr auto expected_ratio = sys::Platform == sys::PlatformType::Windows ? 2.5 : 10.0;
    TEST_ASSERT_GREATER(ratio, expected_ratio);
    #endif
}

TEST_CASE(Test_simd_Sin_small)
{
    constexpr size_t iterations = 2;
    const auto inputs = make_values<float>(iterations);
    std::vector<float> results(inputs.size());
    
    simd::Sin(sys::make_span(inputs), sys::make_span(results));
    test_simd_Sin_almost_equal(testName, &(results[0]));
    const size_t values_size = inputs.size() / iterations;
    test_simd_Sin_almost_equal(testName, &(results[values_size]));
    
    // Be sure the end of the array is calculated correctly.
    for (size_t count = 0; count < values_size; count++)
    {
        const size_t small_count = (values_size * (iterations - 1)) + count;

        const std::span<const float> s(inputs.data(), small_count);
        simd::Sin(s, sys::make_span(results));
        test_simd_Sin_almost_equal(testName, &(results[values_size]), count);    
    }
}

TEST_CASE(Test_simd_Sin_Cos)
{
    constexpr size_t iterations = sys::release ? 2000000 : 200;

    const auto inputs = make_values<float>(iterations);
    std::vector<float> results(inputs.size());

    simd::Sin(sys::make_span(inputs), sys::make_span(results));
    slow_Sin(sys::make_span(inputs), sys::make_span(results));

    auto start = std::chrono::steady_clock::now();
    simd::Sin(sys::make_span(inputs), sys::make_span(results));
    simd::Cos(sys::make_const_span(results), sys::make_span(results));
    auto end = std::chrono::steady_clock::now();
    const std::chrono::duration<double> elapsed_simd = end - start;

    start = std::chrono::steady_clock::now();
    slow_Sin(sys::make_span(inputs), sys::make_span(results));
    slow_Cos(sys::make_const_span(results), sys::make_span(results));
    end = std::chrono::steady_clock::now();
    const std::chrono::duration<double> elapsed_slow = end - start;

    #if NDEBUG // DEBUG SIMD code is slow
    const auto ratio = elapsed_slow / elapsed_simd;
    // Ratios observed by testing
    constexpr auto expected_ratio = sys::Platform == sys::PlatformType::Windows ? 2.5 : 10.0;
    TEST_ASSERT_GREATER(ratio, expected_ratio);
    #endif
    TEST_ASSERT_TRUE(true);
}


template<typename T>
static void slow_Arg(coda_oss::span<const std::complex<T>> inputs, coda_oss::span<T> outputs)
{
    // This is intentionally slow so that we have something to compare with SIMD results
    auto begin = inputs.begin();
    const auto end = inputs.end();
    auto it = outputs.begin();
    while (begin != end)
    {
        *it++ = std::arg(*begin++);
    }
}

TEST_CASE(Test_simd_Arg)
{
    constexpr size_t iterations = sys::release ? 4000000 : 400;

    const auto inputs_ = make_values<float>(iterations);
    std::vector<std::complex<float>> inputs;
    inputs.reserve(inputs_.size());
    for (auto&& v : inputs_)
    {
        inputs.emplace_back(v, -v);
    }

    std::vector<float> results(inputs.size());
    auto start = std::chrono::steady_clock::now();
    simd::Arg(sys::make_const_span(inputs), sys::make_span(results));
    auto end = std::chrono::steady_clock::now();
    const std::chrono::duration<double> elapsed_simd = end - start;

    std::vector<float> slow_results(inputs.size());
    start = std::chrono::steady_clock::now();
    slow_Arg(sys::make_const_span(inputs), sys::make_span(slow_results));
    end = std::chrono::steady_clock::now();
    const std::chrono::duration<double> elapsed_slow = end - start;

    #if NDEBUG // DEBUG SIMD code is slow
    const auto ratio = elapsed_slow / elapsed_simd;
    // Ratios observed by testing
    constexpr auto expected_ratio = sys::Platform == sys::PlatformType::Windows ? 2.5 : 10.0;
    TEST_ASSERT_GREATER(ratio, expected_ratio);
    #endif

    for (size_t i = 0; i < results.size(); i++)
    {
        TEST_ASSERT_ALMOST_EQ(results[i], slow_results[i]);
    }
}

TEST_MAIN(

    TEST_CHECK(Test_simd_Sin);
    TEST_CHECK(Test_simd_Sin_small);

    TEST_CHECK(Test_simd_Sin_Cos);
    
    TEST_CHECK(Test_simd_Arg);
)
