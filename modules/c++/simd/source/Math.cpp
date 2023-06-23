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
#include <stdexcept>
#include <limits>
#include <functional>

#include "sys/OS.h"
#include "gsl/gsl.h"

// We'll sort this out a runtime, not compile-time; see instrset.h:
//     The following values of INSTRSET are currently defined:
//     [...]
//     9:  AVX512F
#ifndef INSTRSET
// This creates compilier errors/warnings with both GCC and MSVC
// that need to eventually be sorted out ... until then ...
//#define INSTRSET 9 // defined(__AVX512F__ ) || defined ( __AVX512__ )
#endif
#include "Vec.h"

template<typename T>
using span = simd::span<T>;

inline auto get_instruction_set()
{
    static const sys::OS os;
    return os.getSIMDInstructionSet();
}

/*
* Table 2.2 from https://github.com/vectorclass/manual/raw/master/vcl_manual.pdf
Vector class    Precision   Elements per vector     Total bits      Minimum recommended instruction set
Vec4f           single          4 			128 		SSE2
Vec2d           double       	2 			128 		SSE2
Vec8f           single          8 			256 		AVX
Vec4d           double          4 			256 		AVX
Vec16f          single          16 			512 		AVX512
Vec8d           double          8 			512 		AVX512
*/
template <typename T, sys::SIMDInstructionSet> constexpr size_t Elements_per_vector();
template <> constexpr size_t Elements_per_vector<float, sys::SIMDInstructionSet::SSE2>() { return 4; }
template <> constexpr size_t Elements_per_vector<double, sys::SIMDInstructionSet::SSE2>() { return 2; }
template <> constexpr size_t Elements_per_vector<float, sys::SIMDInstructionSet::AVX2>() { return 8; }
template <> constexpr size_t Elements_per_vector<double, sys::SIMDInstructionSet::AVX2>() { return 4; }
template <> constexpr size_t Elements_per_vector<float, sys::SIMDInstructionSet::AVX512F>() { return 16; }
template <> constexpr size_t Elements_per_vector<double, sys::SIMDInstructionSet::AVX512F>() { return 8; }

template<typename T>
inline size_t getWidth()
{
    switch (get_instruction_set())
    {
    case sys::SIMDInstructionSet::SSE2: return Elements_per_vector<T, sys::SIMDInstructionSet::SSE2>();
    case sys::SIMDInstructionSet::AVX2: return Elements_per_vector<T, sys::SIMDInstructionSet::AVX2>();
    case sys::SIMDInstructionSet::AVX512F: return Elements_per_vector<T, sys::SIMDInstructionSet::AVX512F>();
    default: break;
    }
    throw std::logic_error("Unexpected sys::SIMDInstructionSet value.");
}

template <typename T1, typename U, typename T2 = T1>
static void validate_inputs(span<const T1> x_values, span<const T2> y_values, span<U> outputs)
{
    if (!y_values.empty() && (x_values.size() != y_values.size()))
    {
        throw std::invalid_argument("inputs aren't the same size");
    }
    if (outputs.size() < x_values.size())
    {
        throw std::invalid_argument("'outputs' is smaller than 'x_values'");
    }
}

// Repeatedly load the appropriate `Vec`s with the inputs (`y_values` may
// be empty) and call the given function `f` (which will end up in SIMD code!).
// The results are stored in `outputs`.
// 
// This the actual workhorse function where most of the "interesting" stuff
// happens; much of the other code is "just" type manipulation.
template <size_t width, typename T1, typename TFunc, typename U = T1, typename T2 = T1>
inline void vec_Func(span<const T1> x_values, span<const T2> y_values, span<U> outputs,
    TFunc f)
{
    validate_inputs(x_values, y_values, outputs);

    simd::Vec<T1, width> x{}; // e.g., vcl::Vec8f
    simd::Vec<T2, width> y{};  // e.g., vcl::Vec8f

    using results_t = simd::Vec<U, width>;  // e.g., vcl::Vec8f
    const std::function<results_t(size_t)> invoke_f = [&](size_t) {
        assert(y_values.empty());
        return f(x, y);
    };
    const std::function<results_t(size_t)> load_x_y = [&](size_t i) {
        y.load(&(y_values[i]));  // load_a() requires very strict alignment
        return f(x, y);
    };
    const auto load_and_invoke_f = y_values.empty() ? invoke_f : load_x_y;

    const auto x_values_size = x_values.size() < width ? 0 : x_values.size() - width;  // don't walk off end with `+= width`
    size_t i = 0;
    for (; i < x_values_size; i += width)
    {
        x.load(&(x_values[i]));  // load_a() requires very strict alignment

        const auto results = load_and_invoke_f(i);
        results.store(&(outputs[i]));  // store_a() requires very strict alignment
    }

    // Do the last few an element at a time; using the same `f`
    // as above helps keep results consistent.
    const auto remaining = gsl::narrow<int>(x_values.size() - i);
    x.load_partial(remaining, &(x_values[i]));
    if (!y_values.empty())
    {
        y.load_partial(remaining, &(y_values[i]));
    }
    const auto results = f(x, y);
    results.store_partial(remaining, &(outputs[i]));
}

// "bind" the compile-time `width` to a particular instantiation for the given type `T`.
template <size_t width, typename T1, typename T2, typename U, typename TFunc>
inline auto bind(TFunc f)
{
    // Be sure inputs/outputs are always passed to the lambda, don't want them captured!
    return [&](span<const T1> x_values, span<const T2> y_values, span<U> outputs) {
        return vec_Func<width>(x_values, y_values, outputs, f); // e.g., vec_Func<4>(inputs, outputs, f)
    };
}

template<typename T1, typename TFunc, typename U = T1, typename T2 = T1>
inline void invoke(span<const T1> x_values, span<const T2> y_values, span<U> outputs, TFunc f)
{
    // For the given type and width, return the right function.
    //
    // Each TFunc is a different type even though they have the same signature;
    // this is because they were generated from lambdas (below) and the type of
    // each lambda is unique.  Because of that, `auto` doesn't work since
    // the inferred types are different and incompatible.
    //
    // The fix is to use an actual function pointer instead of lambda.
    using retval_t = std::function<void(span<const T1>, span<const T2>, span<U>)>;
    static const auto get_simd_func = [&f]() ->  retval_t {
        constexpr auto sse2_width = Elements_per_vector<T1,  sys::SIMDInstructionSet::SSE2>();
        constexpr auto avx2_width = Elements_per_vector<T1,  sys::SIMDInstructionSet::AVX2>();
        constexpr auto avx512f_width = Elements_per_vector<T1,  sys::SIMDInstructionSet::AVX512F>();

        // At runtime, once we know we have SSE2/AVX/AVX512, that won't change.
        static const auto width = getWidth<T1>();
        switch (width)
        {
        case sse2_width: return bind<sse2_width, T1, T2, U>(f);
        case avx2_width: return bind<avx2_width, T1, T2, U>(f);
        case avx512f_width: return bind<avx512f_width, T1, T2, U>(f);
        default:  break;
        }
        throw std::logic_error("Unknown 'width' value = " + std::to_string(width));
    };

    // Only need to get the actual function once because the width won't change.
    static const auto func = get_simd_func();
    func(x_values, y_values, outputs);
}
template<typename T, typename TFunc, typename U = T>
inline void invoke(span<const T> inputs, span<U> outputs, TFunc f)
{
    static const span<const T> empty;
    invoke(inputs, empty, outputs, f);
}

void simd::Sin(span<const float> inputs, span<float> outputs)
{
    static const auto f = [](const auto& v, const auto&) { return sin(v); };
    invoke(inputs, outputs, f);
}
void simd::Sin(span<const double> inputs, span<double> outputs)
{
    static const auto f = [](const auto& v, const auto&) { return sin(v); };
    invoke(inputs, outputs, f);
}

void simd::Cos(span<const float> inputs, span<float> outputs)
{
    static const auto f = [](const auto& v, const auto&) { return cos(v); };
    invoke(inputs, outputs, f);
}
void simd::Cos(span<const double> inputs, span<double> outputs)
{
    static const auto f = [](const auto& v, const auto&) { return cos(v); };
    invoke(inputs, outputs, f);
}

void simd::Tan(span<const float> inputs, span<float> outputs)
{
    static const auto f = [](const auto& v, const auto&) { return tan(v); };
    invoke(inputs, outputs, f);
}
void simd::Tan(span<const double> inputs, span<double> outputs)
{
    static const auto f = [](const auto& v, const auto&) { return tan(v); };
    invoke(inputs, outputs, f);
}

void simd::ATan2(span<const float> xValues, span<const float> yValues, span<float> outputs)
{
    static const auto f = [](const auto& x, const auto& y) { return atan2(x, y); };
    invoke(xValues, yValues, outputs, f);
}
void simd::ATan2(span<const double> xValues, span<const double> yValues, span<double> outputs)
{
    static const auto f = [](const auto& x, const auto& y) { return atan2(x, y); };
    invoke(xValues, yValues, outputs, f);
}

void simd::Arg(span<const std::complex<float>> /*zValues*/, span<float> /*outputs*/)
{
    // https://en.cppreference.com/w/cpp/numeric/complex/arg
    // "... as if the function is implemented as std::atan2(std::imag(z), std::real(z))."
}
