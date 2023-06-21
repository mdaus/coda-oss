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

#include "Vec.h"

inline bool isSSE2()
{
    return true;  // TODO
}
inline bool isAVX()
{
    return true;  // TODO
}
inline bool isAVX512()
{
    return false;  // TODO
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
enum class instruction_set
{
    SSE2,
    AVX,
    AVX512,
};
inline instruction_set get_instruction_set()
{
    if (isAVX512())
    {
        return instruction_set::AVX512;
    }
    if (isAVX())
    {
        return instruction_set::AVX;
    }
    if (isSSE2())
    {
        return instruction_set::SSE2;
    }
    throw std::runtime_error("Must have at least SSE2.");
}

template <typename T, instruction_set> constexpr size_t Elements_per_vector();
template <> constexpr size_t Elements_per_vector<float, instruction_set::SSE2>() { return 4; }
template <> constexpr size_t Elements_per_vector<double, instruction_set::SSE2>() { return 2; }
template <> constexpr size_t Elements_per_vector<float, instruction_set::AVX>() { return 8; }
template <> constexpr size_t Elements_per_vector<double, instruction_set::AVX>() { return 4; }
template <> constexpr size_t Elements_per_vector<float, instruction_set::AVX512>() { return 16; }
template <> constexpr size_t Elements_per_vector<double, instruction_set::AVX512>() { return 8; }

template<typename T>
inline size_t Elements_per_vector()
{
    switch (get_instruction_set())
    {
    case instruction_set::SSE2: return Elements_per_vector<T, instruction_set::SSE2>();
    case instruction_set::AVX: return Elements_per_vector<T, instruction_set::AVX>();
    case instruction_set::AVX512: return Elements_per_vector<T, instruction_set::AVX512>();
    default: break;
    }
    throw std::logic_error("Unexpected instruction_set value.");
}

template <size_t width, typename T, typename TFunc>
inline size_t simd_Func(coda_oss::span<const T> inputs, coda_oss::span<T> outputs,
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
    return i;
}

template <typename T, typename TFunc>
inline void slow_Func(size_t i, coda_oss::span<const T> inputs, coda_oss::span<T> outputs,
    TFunc f)
{
    // Finish any remaining one at a time
    for (; i < inputs.size(); i++)
    {
        outputs[i] = f(inputs[i]);
    }
}

template <size_t width, typename T, typename TFuncSimd, typename TFunc>
inline void call_Funcs(coda_oss::span<const T> inputs, coda_oss::span<T> outputs,
    TFuncSimd func_simd, TFunc func)
{
    if (outputs.size() < inputs.size())
    {
        throw std::invalid_argument("'outputs' smaller than 'inputs'");
    }

    #ifndef NDEBUG // i.e., debug, not release
    // The output could be bigger than input; help identify walking off the end.
    for (auto& output : outputs)
    {
        output = std::numeric_limits<T>::min();;
    }
    #endif

    size_t i = simd_Func<width>(inputs, outputs, func_simd);
    slow_Func(i, inputs, outputs, func);
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
using Func_t = std::function<void(coda_oss::span<const T>, coda_oss::span<T>)>;
template <typename T>
inline auto getFuncForWidth(Func_t<T> fSSE2, Func_t<T> fAVX, Func_t<T> fAVX512)
{
    // At runtime, once we know we have SSE2/AVX/AVX512, that won't change.
    static const auto width = Elements_per_vector<T>();
    switch (width)
    {
    case Elements_per_vector<T, instruction_set::SSE2>(): return fSSE2;
    case Elements_per_vector<T, instruction_set::AVX>(): return fAVX;
    case Elements_per_vector<T, instruction_set::AVX512>(): return fAVX512;
    default: break;
    }

     throw std::logic_error("Unknown 'width' value = " + std::to_string(width));
}

template<typename T>
inline void call_sin(coda_oss::span<const T> inputs, coda_oss::span<T> outputs)
{
    static const auto simd_f = [](const auto& v) { return sin(v); };
    static const auto std_f = [](const auto& v) { return sin(v); };

    // Be sure inputs/outputs are always passed to the lambda, don't want them captured!
    static const auto fSSE2 = [](coda_oss::span<const T> inputs, coda_oss::span<T> outputs) {
        return call_Funcs<Elements_per_vector<T, instruction_set::SSE2>()>(inputs, outputs,  simd_f, std_f);
    };
    static const auto fAVX = [](coda_oss::span<const T> inputs, coda_oss::span<T> outputs) {
        return call_Funcs<Elements_per_vector<T, instruction_set::AVX>()>(inputs, outputs, simd_f, std_f);
    };
    static const auto fAVX512 = [](coda_oss::span<const T> inputs, coda_oss::span<T> outputs) {
        return  call_Funcs<Elements_per_vector<T, instruction_set::AVX512>()>(inputs, outputs, simd_f, std_f);
    };

    // During runtime, these functions don't need to change
    // since SSE/AVX/AVX512 won't change.
    static const auto f = getFuncForWidth<T>(fSSE2, fAVX, fAVX512);
    f(inputs, outputs);
}
void simd::Sin(coda_oss::span<const float> inputs, coda_oss::span<float> outputs)
{
    call_sin(inputs, outputs);
}
void simd::Sin(coda_oss::span<const double> inputs, coda_oss::span<double> outputs)
{
    call_sin(inputs, outputs);
}

template<typename T>
inline void call_cos(coda_oss::span<const T> inputs, coda_oss::span<T> outputs)
{
    static const auto simd_f = [](const auto& v) { return cos(v); };
    static const auto std_f = [](const auto& v) { return cos(v); };

    // Be sure inputs/outputs are always passed to the lambda, don't want them captured!
    static const auto fSSE2 = [](coda_oss::span<const T> inputs, coda_oss::span<T> outputs) {
        return call_Funcs<Elements_per_vector<T, instruction_set::SSE2>()>(inputs, outputs,  simd_f, std_f);
    };
    static const auto fAVX = [](coda_oss::span<const T> inputs, coda_oss::span<T> outputs) {
        return call_Funcs<Elements_per_vector<T, instruction_set::AVX>()>(inputs, outputs, simd_f, std_f);
    };
    static const auto fAVX512 = [](coda_oss::span<const T> inputs, coda_oss::span<T> outputs) {
        return  call_Funcs<Elements_per_vector<T, instruction_set::AVX512>()>(inputs, outputs, simd_f, std_f);
    };

    // During runtime, these functions don't need to change
    // since SSE/AVX/AVX512 won't change.
    static const auto f = getFuncForWidth<T>(fSSE2, fAVX, fAVX512);
    f(inputs, outputs);
}
void simd::Cos(coda_oss::span<const float> inputs, coda_oss::span<float> outputs)
{
    call_cos(inputs, outputs);
}
void simd::Cos(coda_oss::span<const double> inputs, coda_oss::span<double> outputs)
{
    call_cos(inputs, outputs);
}

template<typename T>
inline void call_tan(coda_oss::span<const T> inputs, coda_oss::span<T> outputs)
{
    static const auto simd_f = [](const auto& v) { return tan(v); };
    static const auto std_f = [](const auto& v) { return tan(v); };

    // Be sure inputs/outputs are always passed to the lambda, don't want them captured!
    static const auto fSSE2 = [](coda_oss::span<const T> inputs, coda_oss::span<T> outputs) {
        return call_Funcs<Elements_per_vector<T, instruction_set::SSE2>()>(inputs, outputs,  simd_f, std_f);
    };
    static const auto fAVX = [](coda_oss::span<const T> inputs, coda_oss::span<T> outputs) {
        return call_Funcs<Elements_per_vector<T, instruction_set::AVX>()>(inputs, outputs, simd_f, std_f);
    };
    static const auto fAVX512 = [](coda_oss::span<const T> inputs, coda_oss::span<T> outputs) {
        return  call_Funcs<Elements_per_vector<T, instruction_set::AVX512>()>(inputs, outputs, simd_f, std_f);
    };

    // During runtime, these functions don't need to change
    // since SSE/AVX/AVX512 won't change.
    static const auto f = getFuncForWidth<T>(fSSE2, fAVX, fAVX512);
    f(inputs, outputs);
}
void simd::Tan(coda_oss::span<const float> inputs, coda_oss::span<float> outputs)
{
    call_tan(inputs, outputs);
}
void simd::Tan(coda_oss::span<const double> inputs, coda_oss::span<double> outputs)
{
    call_tan(inputs, outputs);
}