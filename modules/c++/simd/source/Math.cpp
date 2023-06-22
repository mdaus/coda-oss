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
inline size_t Elements_per_vector()
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
    simd::Vec<T, width> vec;  // i.e., vcl::Vec8f

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
inline void call_Func(span<const T> inputs, span<T> outputs, TFunc f)
{
    if (outputs.size() < inputs.size())
    {
        throw std::invalid_argument("'outputs' is smaller than 'inputs'");
    }

    #ifndef NDEBUG // i.e., debug, not release
    // The output could be bigger than input; help identify walking off the end.
    for (auto& output : outputs)
    {
        output = std::numeric_limits<T>::min();;
    }
    #endif

    vec_Func<width>(inputs, outputs, f);
}

// For the given type and width, return the right function.
//
// Each TFunc is a different type even though they have the same signature;
// this is because they were generated from lambdas (below) and the type of
// each lambda is unique.  Because of that, `auto` doesn't work since
// the inferred types are different and incompatible.
//
// The fix is to use an actual function pointer instead of lambda which is
// ever so slightly slower.
template<typename T>
using Func_t = std::function<void(span<const T>, span<T>)>;
template <typename T>
inline auto getFuncForWidth(Func_t<T> fSSE2, Func_t<T> fAVX2, Func_t<T> fAVX512F)
{
    // At runtime, once we know we have SSE2/AVX/AVX512, that won't change.
    static const auto width = Elements_per_vector<T>();
    switch (width)
    {
    case Elements_per_vector<T, sys::SIMDInstructionSet::SSE2>(): return fSSE2;
    case Elements_per_vector<T, sys::SIMDInstructionSet::AVX2>(): return fAVX2;
    case Elements_per_vector<T, sys::SIMDInstructionSet::AVX512F>(): return fAVX512F;
    default: break;
    }

     throw std::logic_error("Unknown 'width' value = " + std::to_string(width));
}

template<typename T>
inline void call_sin(span<const T> inputs, span<T> outputs)
{
    static const auto f = [](const auto& v) { return sin(v); };

    // Be sure inputs/outputs are always passed to the lambda, don't want them captured!
    static const auto fSSE2 = [&](span<const T> inputs, span<T> outputs) {
        return call_Func<Elements_per_vector<T,  sys::SIMDInstructionSet::SSE2>()>(inputs, outputs,  f);
    };
    static const auto fAVX2 = [&](span<const T> inputs, span<T> outputs) {
        return call_Func<Elements_per_vector<T, sys::SIMDInstructionSet::AVX2>()>(inputs, outputs, f);
    };
    static const auto fAVX512F = [&](span<const T> inputs, span<T> outputs) {
        return  call_Func<Elements_per_vector<T, sys::SIMDInstructionSet::AVX512F>()>(inputs, outputs, f);
    };

    // During runtime, these functions don't need to change
    // since SSE/AVX/AVX512 won't change.
    static const auto func = getFuncForWidth<T>(fSSE2, fAVX2, fAVX512F);
    func(inputs, outputs);
}
void simd::Sin(span<const float> inputs, span<float> outputs)
{
    call_sin(inputs, outputs);
}
void simd::Sin(span<const double> inputs, span<double> outputs)
{
    call_sin(inputs, outputs);
}

template<typename T>
inline void call_cos(span<const T> inputs, span<T> outputs)
{
    static const auto f = [](const auto& v) { return cos(v); };

    // Be sure inputs/outputs are always passed to the lambda, don't want them captured!
    static const auto fSSE2 = [](span<const T> inputs, span<T> outputs) {
        return call_Func<Elements_per_vector<T, sys::SIMDInstructionSet::SSE2>()>(inputs, outputs,  f);
    };
    static const auto fAVX2 = [](span<const T> inputs, span<T> outputs) {
        return call_Func<Elements_per_vector<T, sys::SIMDInstructionSet::AVX2>()>(inputs, outputs, f);
    };
    static const auto fAVX512F = [](span<const T> inputs, span<T> outputs) {
        return  call_Func<Elements_per_vector<T, sys::SIMDInstructionSet::AVX512F>()>(inputs, outputs, f);
    };

    // During runtime, these functions don't need to change
    // since SSE/AVX/AVX512 won't change.
    static const auto func = getFuncForWidth<T>(fSSE2, fAVX2, fAVX512F);
    func(inputs, outputs);
}
void simd::Cos(span<const float> inputs, span<float> outputs)
{
    call_cos(inputs, outputs);
}
void simd::Cos(span<const double> inputs, span<double> outputs)
{
    call_cos(inputs, outputs);
}

template<typename T>
inline void call_tan(span<const T> inputs, span<T> outputs)
{
    static const auto f = [](const auto& v) { return tan(v); };

    // Be sure inputs/outputs are always passed to the lambda, don't want them captured!
    static const auto fSSE2 = [](span<const T> inputs, span<T> outputs) {
        return call_Func<Elements_per_vector<T, sys::SIMDInstructionSet::SSE2>()>(inputs, outputs,  f);
    };
    static const auto fAVX2 = [](span<const T> inputs, span<T> outputs) {
        return call_Func<Elements_per_vector<T, sys::SIMDInstructionSet::AVX2>()>(inputs, outputs, f);
    };
    static const auto fAVX512F = [](span<const T> inputs, span<T> outputs) {
        return  call_Func<Elements_per_vector<T, sys::SIMDInstructionSet::AVX512F>()>(inputs, outputs, f);
    };

    // During runtime, these functions don't need to change
    // since SSE/AVX/AVX512 won't change.
    static const auto func = getFuncForWidth<T>(fSSE2, fAVX2, fAVX512F);
    func(inputs, outputs);
}
void simd::Tan(span<const float> inputs, span<float> outputs)
{
    call_tan(inputs, outputs);
}
void simd::Tan(span<const double> inputs, span<double> outputs)
{
    call_tan(inputs, outputs);
}
