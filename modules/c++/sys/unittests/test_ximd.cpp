/* =========================================================================
 * This file is part of sys-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * © Copyright 2024, Maxar Technologies, Inc.
 *
 * sys-c++ is free software; you can redistribute it and/or modify
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

#include <complex>
#include <vector>
#include <std/span>
#include <std/numbers>

#include <sys/Ximd.h>
#include <gsl/gsl.h>
#include <sys/Span.h>

#include "TestCase.h"

using zfloat = std::complex<float>;

using intv = sys::Ximd<int>;
using floatv = sys::Ximd<float>;

// Manage a SIMD complex as an array of two SIMDs
using zfloatv = std::array<floatv, 2>;
static inline auto& real(zfloatv& z) noexcept
{
    return z[0];
}
static inline const auto& real(const zfloatv& z) noexcept
{
    return z[0];
}
static inline auto& imag(zfloatv& z) noexcept
{
    return z[1];
}
static inline const auto& imag(const zfloatv& z) noexcept
{
    return z[1];
}
static inline size_t size(const zfloatv& z) noexcept
{
    auto retval = real(z).size();
    assert(retval == imag(z).size());
    return retval;
}

static inline auto arg(const zfloatv& z)
{
    // https://en.cppreference.com/w/cpp/numeric/complex/arg
    // > `std::atan2(std::imag(z), std::real(z))`
    return atan2(real(z), imag(z));  // arg()
}

template <typename TGeneratorReal, typename TGeneratorImag>
static inline auto make_zfloatv(TGeneratorReal&& generate_real, TGeneratorImag&& generate_imag)
{
    zfloatv retval;
    for (size_t i = 0; i < size(retval); i++)
    {
        real(retval)[i] = generate_real(i);
        imag(retval)[i] = generate_imag(i);
    }
    return retval;
}

static inline auto load(std::span<const zfloat> p)
{
    const auto generate_real = [&](size_t i) { return p[i].real(); };
    const auto generate_imag = [&](size_t i) { return p[i].imag(); };
    return make_zfloatv(generate_real, generate_imag);
}

TEST_CASE(testDefaultConstructor)
{
    // sanity check implementation and utility routines
    TEST_ASSERT_EQ(floatv::size(), size(zfloatv{}));

    // intv v = 1;
    intv v;
    v = 1;
    for (size_t i = 0; i < v.size(); i++)
    {
        TEST_ASSERT_EQ(1, v[i]);
    }
}

// Sample code from SIX; see **ComplexToAMP8IPHS8I.cpp**.
static inline auto GetPhase(std::complex<double> v)
{
    // There's an intentional conversion to zero when we cast 256 -> uint8. That wrap around
    // handles cases that are close to 2PI.
    double phase = std::arg(v);
    if (phase < 0.0) phase += coda_oss::numbers::pi * 2.0; // Wrap from [0, 2PI]
    return phase;
}
static uint8_t getPhase(zfloat v, float phase_delta)
{
    // Phase is determined via arithmetic because it's equally spaced.
    const auto phase = GetPhase(v);
    return gsl::narrow_cast<uint8_t>(std::round(phase / phase_delta));
}

static inline auto if_add(const sys::ximd_mask& m, const floatv lhs, typename floatv::value_type rhs)
{
    const auto generate_add = [&](size_t i) {
        return m[i] ? lhs[i] + rhs : lhs[i];
    };
    return floatv(generate_add);
}
static inline auto roundi(const floatv& v)  // match vcl::roundi()
{
    const auto rounded = round(v);
    const auto generate_roundi = [&](size_t i)
    { return static_cast<typename intv::value_type>(rounded[i]); };
    return intv(generate_roundi);
}
static inline auto getPhase(const zfloatv& v, float phase_delta)
{
    // Phase is determined via arithmetic because it's equally spaced.
    // There's an intentional conversion to zero when we cast 256 -> uint8. That wrap around
    // handles cases that are close to 2PI.
    auto phase = arg(v);
    //where(phase < 0.0f, phase) += std::numbers::pi_v<float> *2.0f; // Wrap from [0, 2PI]
    phase = if_add(phase < 0.0f, phase, std::numbers::pi_v<float> * 2.0f); // Wrap from [0, 2PI]
    return roundi(phase / phase_delta);
}

static const std::vector<zfloat>& cxValues()
{
    static const std::vector<zfloat> retval{/*{0, 0},*/ {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}};
    return retval;
}

static auto expected_getPhase_values(const std::string& testName, float phase_delta)
{
    auto&& values = cxValues();
    TEST_ASSERT(values.size() % floatv::size() == 0);
    std::vector<uint8_t> expected;
    expected.reserve(values.size());
    for (auto&& v : values)
    {
        expected.push_back(getPhase(v, phase_delta));
    }
    return expected;
}

TEST_CASE(testGetPhase)
{
    constexpr auto phase_delta = 0.01f;
    static const auto expected = expected_getPhase_values(testName, phase_delta);

    std::vector<zfloatv> valuesv;
    constexpr auto sz = floatv::size();
    auto&& values = cxValues();
    auto const pValues = sys::make_span(values);
    auto p = sys::make_span(pValues.data(), sz);
    for (size_t i = 0; i < values.size() / sz; i++)
    {
        valuesv.push_back(load(p));
        p = sys::make_span(p.data() + sz, p.size());
    }

    std::vector<uint8_t> actual;
    for (auto&& zvaluev : valuesv)
    {
        const auto phase = getPhase(zvaluev, phase_delta);
        for (size_t i = 0; i < phase.size(); i++)
        {
            actual.push_back(gsl::narrow_cast<uint8_t>(phase[i]));
        }
    }

    TEST_ASSERT_EQ(actual.size(), expected.size());
    for (size_t i = 0; i < actual.size(); i++)
    {
        TEST_ASSERT_EQ(actual[i], expected[i]);
    }
}


TEST_MAIN(
    TEST_CHECK(testDefaultConstructor);

    TEST_CHECK(testGetPhase);
)
