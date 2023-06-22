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

template <size_t width, typename T, typename TFunc>
inline void vec_Func(span<const T> inputs, span<T> outputs,
    TFunc f)
{
    if (outputs.size() < inputs.size())
    {
        throw std::invalid_argument("'outputs' is smaller than 'inputs'");
    }

    simd::Vec<T, width> vec{};  // i.e., vcl::Vec8f

    const auto inputs_size = inputs.size() < width ? 0 : inputs.size() - width;  // don't walk off end with `+= width`
    size_t i = 0;
    for (; i < inputs_size; i += width)
    {
        auto const pInputs = &(inputs[i]);
        vec.load(pInputs);  // load_a() requires very strict alignment

        const auto results = f(vec);

        auto const pOutputs = &(outputs[i]);
        results.store(pOutputs);  // store_a() requires very strict alignment
    }

    // Do the last few an element at a time; using the same `f`
    // as above helps keep results consistent.
    const auto remaining = gsl::narrow<int>(inputs.size() - i);
    vec.load_partial(remaining, &(inputs[i]));
    const auto results = f(vec);
    results.store_partial(remaining, &(outputs[i]));
}
template <size_t width, typename T, typename TFunc>
inline void vec_Func(span<const T> x_values, span<const T> y_values, span<T> outputs,
    TFunc f)
{
    if (x_values.size() != y_values.size())
    {
        throw std::invalid_argument("inputs aren't the same size");
    }
    if (outputs.size() < x_values.size())
    {
        throw std::invalid_argument("'outputs' is smaller than 'x_values'");
    }

    simd::Vec<T, width> x{};  // i.e., vcl::Vec8f
    simd::Vec<T, width> y{};  // i.e., vcl::Vec8f

    const auto x_values_size = x_values.size() < width ? 0 : x_values.size() - width;  // don't walk off end with `+= width`
    size_t i = 0;
    for (; i < x_values_size; i += width)
    {
        x.load(&(x_values[i]));  // load_a() requires very strict alignment
        y.load(&(y_values[i]));  // load_a() requires very strict alignment

        const auto results = f(x, y);

        auto const pOutputs = &(outputs[i]);
        results.store(pOutputs);  // store_a() requires very strict alignment
    }

    // Do the last few an element at a time; using the same `f`
    // as above helps keep results consistent.
    const auto remaining = gsl::narrow<int>(x_values.size() - i);
    x.load_partial(remaining, &(x_values[i]));
    y.load_partial(remaining, &(y_values[i]));
    const auto results = f(x, y);
    results.store_partial(remaining, &(outputs[i]));
}


// "bind" the compile-time `width` to a particular insantiation for the given type `T`.
template <size_t width, typename T, typename TFunc>
inline auto bind(TFunc f)
{
    // Be sure inputs/outputs are always passed to the lambda, don't want them captured!
    return [&](span<const T> inputs, span<T> outputs) {
        return vec_Func<width>(inputs, outputs, f); // e.g., vec_Func<4>(inputs, outputs, f)
    };
}
// "bind" the compile-time `width` to a particular insantiation for the given type `T`.
template <size_t width, typename T, typename TFunc2>
inline auto bind2(TFunc2 f)
{
    // Be sure inputs/outputs are always passed to the lambda, don't want them captured!
    return [&](span<const T> x_values, span<const T> y_values, span<T> outputs) {
        return vec_Func<width>(x_values, y_values, outputs, f); // e.g., vec_Func<4>(inputs, outputs, f)
    };
}

template<typename T, typename TFunc>
inline void invoke(span<const T> inputs, span<T> outputs, TFunc f)
{
    // For the given type and width, return the right function.
    //
    // Each TFunc is a different type even though they have the same signature;
    // this is because they were generated from lambdas (below) and the type of
    // each lambda is unique.  Because of that, `auto` doesn't work since
    // the inferred types are different and incompatible.
    //
    // The fix is to use an actual function pointer instead of lambda.
    using retval_t = std::function<void(span<const T>, span<T>)>;
    static const auto get_simd_func = [&f]() ->  retval_t {
        constexpr auto sse2_width = Elements_per_vector<T,  sys::SIMDInstructionSet::SSE2>();
        constexpr auto avx2_width = Elements_per_vector<T,  sys::SIMDInstructionSet::AVX2>();
        constexpr auto avx512f_width = Elements_per_vector<T,  sys::SIMDInstructionSet::AVX512F>();

        // At runtime, once we know we have SSE2/AVX/AVX512, that won't change.
        static const auto width = getWidth<T>();
        switch (width)
        {
        case sse2_width: return bind<sse2_width, T>(f);
        case avx2_width: return bind<avx2_width, T>(f);
        case avx512f_width: return bind<avx512f_width, T>(f);
        default:  break;
        }
        throw std::logic_error("Unknown 'width' value = " + std::to_string(width));
    };

    // Only need to get the actual function once because the width won't change.
    static const auto func = get_simd_func();
    func(inputs, outputs);
}

template<typename T, typename TFunc>
inline void invoke(span<const T> x_values, span<const T> y_values, span<T> outputs, TFunc f)
{
    // For the given type and width, return the right function.
    //
    // Each TFunc is a different type even though they have the same signature;
    // this is because they were generated from lambdas (below) and the type of
    // each lambda is unique.  Because of that, `auto` doesn't work since
    // the inferred types are different and incompatible.
    //
    // The fix is to use an actual function pointer instead of lambda.
    using retval_t = std::function<void(span<const T>, span<const T>, span<T>)>;
    static const auto get_simd_func = [&f]() ->  retval_t {
        constexpr auto sse2_width = Elements_per_vector<T,  sys::SIMDInstructionSet::SSE2>();
        constexpr auto avx2_width = Elements_per_vector<T,  sys::SIMDInstructionSet::AVX2>();
        constexpr auto avx512f_width = Elements_per_vector<T,  sys::SIMDInstructionSet::AVX512F>();

        // At runtime, once we know we have SSE2/AVX/AVX512, that won't change.
        static const auto width = getWidth<T>();
        switch (width)
        {
        case sse2_width: return bind2<sse2_width, T>(f);
        case avx2_width: return bind2<avx2_width, T>(f);
        case avx512f_width: return bind2<avx512f_width, T>(f);
        default:  break;
        }
        throw std::logic_error("Unknown 'width' value = " + std::to_string(width));
    };

    // Only need to get the actual function once because the width won't change.
    static const auto func = get_simd_func();
    func(x_values, y_values, outputs);
}


void simd::Sin(span<const float> inputs, span<float> outputs)
{
    static const auto f = [](const auto& v) { return sin(v); };
    invoke(inputs, outputs, f);
}
void simd::Sin(span<const double> inputs, span<double> outputs)
{
    static const auto f = [](const auto& v) { return sin(v); };
    invoke(inputs, outputs, f);
}

void simd::Cos(span<const float> inputs, span<float> outputs)
{
    static const auto f = [](const auto& v) { return cos(v); };
    invoke(inputs, outputs, f);
}
void simd::Cos(span<const double> inputs, span<double> outputs)
{
    static const auto f = [](const auto& v) { return cos(v); };
    invoke(inputs, outputs, f);
}

void simd::Tan(span<const float> inputs, span<float> outputs)
{
    static const auto f = [](const auto& v) { return tan(v); };
    invoke(inputs, outputs, f);
}
void simd::Tan(span<const double> inputs, span<double> outputs)
{
    static const auto f = [](const auto& v) { return tan(v); };
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