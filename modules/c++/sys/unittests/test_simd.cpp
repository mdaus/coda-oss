/* =========================================================================
 * This file is part of sys-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2016, MDA Information Systems LLC
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

#include <string>
#include <std/simd>
#include <array>
#include <std/span>
#include <std/numbers>
#include <complex>
using zfloat = std::complex<float>;

#include <sys/OS.h>
#include <gsl/gsl.h>
#include "TestCase.h"

TEST_CASE(test_SIMD_Instructions)
{
    const sys::OS os;
    const auto simdInstructionSet = os.getSIMDInstructionSet();

    const auto isSSE2 = simdInstructionSet == sys::SIMDInstructionSet::SSE2;
    const auto isAVX2 = simdInstructionSet == sys::SIMDInstructionSet::AVX2;
    const auto isAVX512F = simdInstructionSet == sys::SIMDInstructionSet::AVX512F;
    TEST_ASSERT(isSSE2 || isAVX2 || isAVX512F);
}

using intv = stdx::simd::simd<int>;
using floatv = stdx::simd::rebind_simd_t<float, intv>;

template <typename T, typename Abi>
inline int ssize(const vcl::simd::basic_simd<T, Abi>& v) noexcept
{
    return v.size();
}

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
static inline auto ssize(const zfloatv& z) noexcept
{
    return static_cast<int>(size(z));
}

/***********************************************************************************************/

// https://en.cppreference.com/w/cpp/numeric/complex/arg
static inline auto arg(const zfloatv& z)
{
    // > `std::atan2(std::imag(z), std::real(z))`
    return atan2(imag(z), real(z));  // arg()
}

template <typename TGeneratorReal, typename TGeneratorImag>
static inline auto generate(TGeneratorReal&& generate_real, TGeneratorImag&& generate_imag)
{
    zfloatv retval;

    using Vec = floatv::native_type;
    auto rv = static_cast<Vec>(real(retval));
    auto iv = static_cast<Vec>(imag(retval));

    for (int i = 0; i < size(retval); i++)
    {
        rv.insert(i, generate_real(i));
        iv.insert(i, generate_imag(i));
    }

    real(retval) = rv;
    imag(retval) = iv;

    return retval;
}
template <typename It>
static inline auto copy_from(zfloatv& this_, It first)
{
    // "`It` satisfies contiguous_iterator."
    const auto mem = &(*first);

    const auto generate_real = [&](size_t i) { return mem[i].real(); };
    const auto generate_imag = [&](size_t i) { return mem[i].imag(); };
    this_ = generate(generate_real, generate_imag);
}

template<size_t N>
static inline auto lookup(intv const indexv, std::span<const float> floats)
{
    assert(N == floats.size());
    return floatv{ vcl::lookup<N>(static_cast<intv::native_type>(indexv), floats.data()) };
}

/***********************************************************************************************/

template<typename TMask, typename TSimd>
static auto simd_select_(const TMask& m, const TSimd& t, const TSimd& f)
{
    auto retval = simd_select(m, t, f);

    for (int i = 0; i < retval.size(); i++)
    {
        const auto result = m[i] ? t[i] : f[i];

        const auto& retval_ = retval;
        const auto r = retval_[i];
        static const std::string testName("simd_select_");
        TEST_ASSERT_EQ(r, result);
    }

    return retval;
}

/***********************************************************************************************/

// Sample code from SIX
static constexpr size_t AmplitudeTableSize = 256;

static auto GetPhase(std::complex<double> v)
{
    // There's an intentional conversion to zero when we cast 256 -> uint8. That wrap around
    // handles cases that are close to 2PI.
    double phase = std::arg(v);
    if (phase < 0.0) phase += std::numbers::pi * 2.0; // Wrap from [0, 2PI]
    return phase;
}
static auto getPhase(zfloat v, float phase_delta)
{
    // Phase is determined via arithmetic because it's equally spaced.
    const auto phase = GetPhase(v);
    return static_cast<uint8_t>(std::round(phase / phase_delta));
}

static auto getPhase(const zfloatv& v, float phase_delta)
{
    // Phase is determined via arithmetic because it's equally spaced.
    // There's an intentional conversion to zero when we cast 256 -> uint8. That wrap around
    // handles cases that are close to 2PI.
    constexpr auto two_pi = std::numbers::pi_v<float> * 2.0f;

    auto phase = arg(v);
    phase = simd_select_(phase < 0.0f, phase + two_pi, phase);
    //phase += simd_select(phase < 0.0f, std::numbers::pi_v<float> *2.0f, 0.0f);
    return lround(phase / phase_delta);
}

static const auto& cxValues()
{
    //static const std::vector<zfloat> retval{/*{0, 0},*/ {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}};
    static const std::vector<zfloat> retval{{0.0, 0.0}, {1.0, 1.0}, {10.0, -10.0}, {-100.0, 100.0}, {-1000.0, -1000.0}, // sample data from SIX
            {-1.0, -1.0}, {1.0, -1.0}, {-1.0, 1.0} // "pad" to multiple of floatv::size()
    };
    return retval;
}

static auto expected_getPhase_values(const std::vector<zfloat>& values, float phase_delta)
{
    static const std::string testName("expected_getPhase_values");
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
    zfloat S(static_cast<float>(A * cos_angle), static_cast<float>(A * sin_angle));
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
    static const auto retval = static_cast<float>(p1 - p0);
    return retval;
}

static auto load(const std::vector<zfloat>& values)
{
    std::vector<zfloatv> retval;
    constexpr auto sz = floatv::size();
    std::span pValues(values);
    std::span p(pValues.data(), sz);
    for (size_t i = 0; i < values.size() / sz; i++)
    {
        zfloatv z;
        copy_from(z, p.begin());
        retval.push_back(z);
        p = std::span(p.data() + sz, p.size());
    }
    return retval;
}

static auto copy_to(intv v)
{
    std::vector<int> retval(v.size());
    v.copy_to(retval.begin());
    return retval;
}

static auto getPhase_(const zfloatv& v, float phase_delta)
{
    auto retval = getPhase(v, phase_delta);

    for (int i = 0; i < retval.size(); i++)
    {
        const auto& r = real(v);
        const auto& j = imag(v);

        const std::complex<float> v_(r[i], j[i]);
        const auto result = getPhase(v_, phase_delta);

        const auto& retval_ = retval;
        static const std::string testName("getPhase_");
        TEST_ASSERT_EQ(retval_[i], result);
    }

    return retval;
}

TEST_CASE(testGetPhase)
{
    const auto& expected = expected_getPhase_values(cxValues(), phase_delta());

    const auto valuesv = load(cxValues());
    std::vector<intv::value_type> actual;
    for (auto&& zvaluev : valuesv)
    {
        const auto phasev = getPhase_(zvaluev, phase_delta());
        for (auto&& phase : copy_to(phasev))
        {
            actual.push_back(phase);
        }
    }

    TEST_ASSERT_EQ(actual.size(), expected.size());
    for (size_t i = 0; i < actual.size(); i++)
    {
        TEST_ASSERT_EQ(actual[i], expected[i]);
    }
}

// Again, more sample code from SIX
struct PhaseDirections final
{
    std::array<zfloat, AmplitudeTableSize> value; // interleaved, std::complex<float>
    std::array<float, AmplitudeTableSize> real;
    std::array<float, AmplitudeTableSize> imag;
};
static auto getPhaseDirections_()
{
    //! Unit vector rays that represent each direction that phase can point.
    PhaseDirections phase_directions;

    const auto p0 = GetPhase(toComplex(1, 0));
    for (size_t i = 0; i < AmplitudeTableSize; i++)
    {
        const float angle = static_cast<float>(p0) + i * phase_delta();
        const auto y = sin(angle);
        const auto x = cos(angle);
        phase_directions.value[i] = { x, y };

        phase_directions.real[i] = phase_directions.value[i].real();
        phase_directions.imag[i] = phase_directions.value[i].imag();
    }
    return phase_directions;
}
static inline const auto& getPhaseDirections()
{
    static const auto retval = getPhaseDirections_();
    return retval;
}

template<size_t N>
static inline auto lookup_(intv const indexv, std::span<const float> floats)
{
    auto retval = lookup<N>(indexv, floats);

    for (int i = 0; i < retval.size(); i++)
    {
        const auto i_ = static_cast<uint8_t>(indexv[i]);
        const auto result = floats[i_];

        static const std::string testName("lookup_");
        const auto& retval_ = retval;
        TEST_ASSERT_EQ(retval_[i], result);
    }

    return retval;
}

TEST_CASE(testLookup)
{
    const auto& phase_directions = getPhaseDirections();

    static const auto& expected_getPhase = expected_getPhase_values(cxValues(), phase_delta());
    std::vector<zfloat> expected;
    for (auto&& phase : expected_getPhase)
    {
        expected.push_back(phase_directions.value[phase]);
    }
    
    const auto valuesv = load(cxValues());
    std::vector<zfloat> actual;
    for (auto&& zvaluev : valuesv)
    {
        const auto phase = getPhase_(zvaluev, phase_delta());
        const auto phase_direction_real = lookup_<AmplitudeTableSize>(phase, phase_directions.real);
        const auto phase_direction_imag = lookup_<AmplitudeTableSize>(phase, phase_directions.imag);

        assert(phase_direction_real.size() == phase_direction_imag.size());
        for (int i = 0; i < phase_direction_real.size(); i++)
        {
            actual.emplace_back(phase_direction_real[i], phase_direction_imag[i]);
        }
    }

    TEST_ASSERT_EQ(actual.size(), expected.size());
    for (size_t i = 0; i < actual.size(); i++)
    {
        TEST_ASSERT_EQ(actual[i], expected[i]);
    }
}


// And ... more sample code from SIX
static auto iota_0_256_()
{
    static_assert(sizeof(size_t) > sizeof(uint8_t), "size_t can't hold UINT8_MAX!");

    std::vector<uint8_t> retval;
    retval.reserve(UINT8_MAX + 1);
    for (size_t i = 0; i <= UINT8_MAX; i++) // Be careful with indexing so that we don't wrap-around in the loop.
    {
        retval.push_back(static_cast<uint8_t>(i));
    }
    assert(retval.size() == UINT8_MAX + 1);
    return retval;
}
static inline std::vector<uint8_t> iota_0_256()
{
    static const auto retval = iota_0_256_();
    return retval;
}

static auto make_magnitudes_()
{
    std::vector<float> result;
    result.reserve(AmplitudeTableSize);
    for (const auto amplitude : iota_0_256())
    {
        // AmpPhase -> Complex
        const auto phase = amplitude;
        const auto complex = toComplex(amplitude, phase);
        result.push_back(std::abs(complex));
    }

    // I don't know if we can guarantee that the amplitude table is non-decreasing.
    // Check to verify property at runtime.
    if (!std::is_sorted(result.begin(), result.end()))
    {
        throw std::runtime_error("magnitudes must be sorted");
    }

    std::array<float, AmplitudeTableSize> retval;
    std::copy(result.begin(), result.end(), retval.begin());
    return retval;
}
static inline std::span<const float> magnitudes()
{
    //! The sorted set of possible magnitudes order from small to large.
    static const auto cached_magnitudes = make_magnitudes_();
    return std::span(cached_magnitudes);
}

/*!
 * Find the nearest element given an iterator range.
 * @param value query value
 * @return index of nearest value within the iterator range.
 */
static uint8_t nearest(std::span<const float> magnitudes, float value)
{
    assert(magnitudes.size() == AmplitudeTableSize);
  
    const auto begin = magnitudes.begin();
    const auto end = magnitudes.end();

    const auto it = std::lower_bound(begin, end, value);
    if (it == begin) return 0;

    const auto prev_it = std::prev(it);
    const auto nearest_it = it == end ? prev_it  :
        (value - *prev_it <= *it - value ? prev_it : it);
    const auto distance = std::distance(begin, nearest_it);
    assert(distance <= std::numeric_limits<uint8_t>::max());
    return static_cast<uint8_t>(distance);
}
static uint8_t find_nearest(zfloat phase_direction, zfloat v)
{
    // We have to do a 1D nearest neighbor search for magnitude.
    // But it's not the magnitude of the input complex value - it's the projection of
    // the complex value onto the ray of candidate magnitudes at the selected phase.
    // i.e. dot product.
    const auto projection = (phase_direction.real() * v.real()) + (phase_direction.imag() * v.imag());
    //assert(std::abs(projection - std::abs(v)) < 1e-5); // TODO ???
    return nearest(magnitudes(), projection);
}

static auto select_(bool c, ptrdiff_t a, ptrdiff_t b)
{
    return c ? a : b;
}

// https://en.cppreference.com/w/cpp/algorithm/lower_bound
static auto my_lower_bound(std::span<const float> magnitudes, const float& value)
{
    assert(magnitudes.size() == AmplitudeTableSize);
    auto first = std::distance(magnitudes.begin(), magnitudes.begin());
    const auto last = std::distance(magnitudes.begin(), magnitudes.end());

    auto count = last - first;
    while (count > 0)
    {
        auto it = first;
        const auto step = count / 2;
        it += step; // std::advance(it, step);

        //if (magnitudes[it] < value)
        //{
        //    first = ++it;
        //    count -= step + 1;
        //}
        //else
        //    count = step;

        auto count_ = count; count_ -= step + 1;  // count -= step + 1;

        const auto c = magnitudes[it];
        const auto test = c < value;
        first = select_(test, ++it, first); // first = ++it;
        count = select_(test, count_, step); // count -= step + 1 <...OR...> count = step
    }

    return first;
}

static auto lower_boundv(std::span<const float> magnitudes, const floatv& v)
{
    assert(magnitudes.size() == AmplitudeTableSize);
    intv first = gsl::narrow<int>(std::distance(magnitudes.begin(), magnitudes.begin()));
    const intv last = gsl::narrow<int>(std::distance(magnitudes.begin(), magnitudes.end()));

    auto count = last - first;
    while (any_of(count > 0))
    {
        //auto it = first;
        const auto step = count / 2;
        auto it = simd_select(count > 0, first + step, first); // std::advance(it, step);

        //if (magnitudes[it] < value)
        //{
        //    first = ++it;
        //    count -= step + 1;
        //}
        //else
        //    count = step;
        const auto c = lookup_<AmplitudeTableSize>(it, magnitudes); // magnituides[it]

        const auto test = (count > 0) && (c < v);
        first = simd_select_(test, ++it, first); // first = ++it;
        auto count_ = count; count_ -= step + 1;  // ...  -= step + 1;
        count = simd_select_(test, count_, step); // count -= step + 1 <...OR...> count = step
    }
    return first;
}

static auto lower_bound_(std::span<const float> magnitudes, const floatv& value)
{
    auto retval = lower_boundv(magnitudes, value);

    for (int i = 0; i < retval.size(); i++)
    {
        const auto v = value[i];
        const auto result = my_lower_bound(magnitudes, v);

        const auto& retval_ = retval;
        const auto r = retval_[i];
        static const std::string testName("lower_bound_");
        TEST_ASSERT_EQ(r, result);
    }

    return retval;
}

static auto nearest(std::span<const float> magnitudes, const floatv& value)
{
    assert(magnitudes.size() == AmplitudeTableSize);

    /*
    const auto it = std::lower_bound(begin, end, value);
    if (it == begin) return 0;

    const auto prev_it = std::prev(it);
    const auto nearest_it = it == end ? prev_it  :
        (value - *prev_it <= *it - value ? prev_it : it);
    const auto distance = std::distance(begin, nearest_it);
    assert(distance <= std::numeric_limits<uint8_t>::max());
    return gsl::narrow<uint8_t>(distance);
    */
    const auto it = lower_bound_(magnitudes, value);

    static const intv begin = 0;
    static const auto& zero = begin;
    if (all_of(it == begin))
    {
        return zero;
    }

    const auto prev_it = simd_select(it == begin, zero, it - 1); // const auto prev_it = std::prev(it);

    const intv end = static_cast<int>(magnitudes.size());
    if (all_of(it == end))
    {
        return prev_it;
    }

    const auto v0 = value - lookup_<AmplitudeTableSize>(prev_it, magnitudes); // value - *prev_it
    const auto v1 = lookup_<AmplitudeTableSize>(it, magnitudes) - value; // *it - value
    //const auto nearest_it = select(v0 <= v1, prev_it, it); //  (value - *prev_it <= *it - value ? prev_it : it);

    //const auto end_test = select(it == end, prev_it, nearest_it); // it == end ? prev_it  : ...
    auto retval = simd_select(it == begin, zero, // if (it == begin) return 0;
        simd_select(it == end, prev_it,  // it == end ? prev_it  : ...
            simd_select(v0 <= v1, prev_it, it) //  (value - *prev_it <= *it - value ? prev_it : it);
        ));
    return retval;
}

static auto nearest_(std::span<const float> magnitudes, const floatv& value)
{
    auto retval = nearest(magnitudes, value);

    for (int i = 0; i < retval.size(); i++)
    {
        const auto v = value[i];
        const auto result = nearest(magnitudes, v);

        const auto& retval_ = retval;
        const auto r = retval_[i];
        static const std::string testName("nearest_");
        TEST_ASSERT_EQ(r, result);
    }

    return retval;
}

static auto find_nearest(std::span<const float> magnitudes,
    const floatv& phase_direction_real, const floatv& phase_direction_imag,
    const zfloatv& v)
{
    // We have to do a 1D nearest neighbor search for magnitude.
    // But it's not the magnitude of the input complex value - it's the projection of
    // the complex value onto the ray of candidate magnitudes at the selected phase.
    // i.e. dot product.
    const auto projection = (phase_direction_real * real(v)) + (phase_direction_imag * imag(v));
    //assert(std::abs(projection - std::abs(v)) < 1e-5); // TODO ???
    return nearest_(magnitudes, projection);
}

TEST_CASE(testFindNearest)
{
    const auto& values = cxValues();
    const auto& phase_directions = getPhaseDirections();

    static const auto& expected_getPhase = expected_getPhase_values(cxValues(), phase_delta());
    std::vector<zfloat> expected_phase_directions;
    for (auto&& phase : expected_getPhase)
    {
        expected_phase_directions.push_back(phase_directions.value[phase]);
    }
    std::vector<uint8_t> expected;
    for (size_t i = 0; i < values.size(); i++)
    {
        auto a = find_nearest(expected_phase_directions[i], values[i]);
        expected.push_back(a);
    }
    
    const auto valuesv = load(cxValues());
    std::vector<intv::value_type> actual;
    for (auto&& v : valuesv)
    {
        const auto phase = getPhase_(v, phase_delta());
        const auto phase_direction_real = lookup_<AmplitudeTableSize>(phase, phase_directions.real);
        const auto phase_direction_imag = lookup_<AmplitudeTableSize>(phase, phase_directions.imag);
        const auto amplitude = find_nearest(magnitudes(), phase_direction_real, phase_direction_imag, v);

        for (auto&& a : copy_to(amplitude))
        {
            actual.push_back(a);
        }
    }

    TEST_ASSERT_EQ(actual.size(), expected.size());
    for (size_t i = 0; i < actual.size(); i++)
    {
        TEST_ASSERT_EQ(actual[i], expected[i]);
    }
}

TEST_MAIN(
    TEST_CHECK(test_SIMD_Instructions);
    TEST_CHECK(testGetPhase);
    TEST_CHECK(testLookup);
    TEST_CHECK(testFindNearest);
    )
