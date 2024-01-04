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

template<typename T>
static inline auto store(const sys::Ximd<T>& v)
{
    std::vector<typename sys::Ximd<T>::value_type> retval;
    for (size_t i = 0; i < v.size(); i++)
    {
        retval.push_back(v[i]);
    }
    return retval;
}
static inline auto store(const zfloatv& v)
{
    std::vector<zfloat> retval;
    for (size_t i = 0; i < size(v); i++)
    {        
        retval.emplace_back(real(v)[i], imag(v)[i]);
    }
    return retval;
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

static const auto& cxValues()
{
    static const std::vector<zfloat> retval{/*{0, 0},*/ {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}};
    return retval;
}

static auto expected_getPhase_values(const std::string& testName, float phase_delta)
{
    auto&& values = cxValues();
    TEST_ASSERT(values.size() % floatv::size() == 0);
    std::vector<uint8_t> expected;
    for (auto&& v : values)
    {
        expected.push_back(getPhase(v, phase_delta));
    }
    return expected;
}

static auto toComplex_(double A, uint8_t phase)
{
    // The phase values should be read in (values 0 to 255) and converted to float by doing:
    // P = (1 / 256) * input_value
    const double P = (1.0 / 256.0) * phase;

    // To convert the amplitude and phase values to complex float (i.e. real and imaginary):
    // S = A * cos(2 * pi * P) + j * A * sin(2 * pi * P)
    const double angle = 2 * std::numbers::pi * P;
    const auto sin_angle = sin(angle);
    const auto cos_angle = cos(angle);
    zfloat S(gsl::narrow_cast<float>(A * cos_angle), gsl::narrow_cast<float>(A * sin_angle));
    return S;
}
inline static auto toComplex(uint8_t amplitude, uint8_t phase)
{
    // A = input_amplitude(i.e. 0 to 255)
    const double A = amplitude;
    return toComplex_(A, phase);
}

static auto phase_delta()
{
    static const auto p0 = GetPhase(toComplex(1, 0));
    static const auto p1 = GetPhase(toComplex(1, 1));
    assert(p0 == 0.0);
    assert(p1 > p0);
    static const auto retval = gsl::narrow_cast<float>(p1 - p0);
    return retval;
}

static auto load(const std::vector<zfloat>& values)
{
    std::vector<zfloatv> retval;
    constexpr auto sz = floatv::size();
    auto const pValues = sys::make_span(values);
    auto p = sys::make_span(pValues.data(), sz);
    for (size_t i = 0; i < values.size() / sz; i++)
    {
        retval.push_back(load(p));
        p = sys::make_span(p.data() + sz, p.size());
    }
    return retval;
}

TEST_CASE(testGetPhase)
{
    static const auto& expected = expected_getPhase_values(testName, phase_delta());

    const auto valuesv = load(cxValues());
    std::vector<uint8_t> actual;
    for (auto&& zvaluev : valuesv)
    {
        const auto phase = getPhase(zvaluev, phase_delta());
        const auto phase_ = store(phase);
        for (auto&& ph : store(phase))
        {
            actual.push_back(gsl::narrow_cast<uint8_t>(ph));
        }
    }

    TEST_ASSERT_EQ(actual.size(), expected.size());
    for (size_t i = 0; i < actual.size(); i++)
    {
        TEST_ASSERT_EQ(actual[i], expected[i]);
    }
}

// Again, more sample code from SIX
static constexpr size_t AmplitudeTableSize = 256;
static const auto& getPhaseDirections()
{
    //! Unit vector rays that represent each direction that phase can point.
    static std::array<zfloat, AmplitudeTableSize> phase_directions; // interleaved, std::complex<float>

     const auto p0 = GetPhase(toComplex(1, 0));
    for (size_t i = 0; i < AmplitudeTableSize; i++)
    {
        const float angle = static_cast<float>(p0) + i * phase_delta();
        const auto y = sin(angle);
        const auto x = cos(angle);
        phase_directions[i] = {x, y};
    }
    return phase_directions;
}

template <size_t N>
static inline auto lookup(const intv& zindex, const std::array<zfloat, N>& phase_directions)
{
    const auto generate_real = [&](size_t i)
    {
        const auto i_ = zindex[i];
        return phase_directions[i_].real();
    };
    const auto generate_imag = [&](size_t i)
    {
        const auto i_ = zindex[i];
        return phase_directions[i_].imag();
    };
    return make_zfloatv(generate_real, generate_imag);
}

TEST_CASE(testLookup)
{
    const auto& phase_directions = getPhaseDirections();

    static const auto& expected_getPhase = expected_getPhase_values(testName, phase_delta());
    std::vector<zfloat> expected;
    for (auto&& phase : expected_getPhase)
    {
        expected.push_back(phase_directions[phase]);
    }
    
    const auto valuesv = load(cxValues());
    std::vector<zfloat> actual;
    for (auto&& zvaluev : valuesv)
    {
        const auto phase = getPhase(zvaluev, phase_delta());
        const auto phase_direction = lookup<AmplitudeTableSize>(phase, phase_directions);

        for (auto&& pd : store(phase_direction))
        {
            actual.push_back(pd);
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
    TEST_CHECK(testLookup);
)
