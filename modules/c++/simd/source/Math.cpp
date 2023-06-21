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
template<typename T>
constexpr size_t getSSE2Width();
template <typename T>
constexpr size_t getAVXWidth();
template <typename T>
constexpr size_t getAVX512Width();

template<> constexpr size_t getSSE2Width<float>()
{
    return 4;
}
template <> constexpr size_t getSSE2Width<double>()
{
    return 2;
}

template<> constexpr size_t getAVXWidth<float>()
{
    return 8;
}
template <> constexpr size_t getAVXWidth<double>()
{
    return 4;
}

template<> constexpr size_t getAVX512Width<float>()
{
    return 16;
}
template <> constexpr size_t getAVX512Width<double>()
{
    return 8;
}

template<typename T>
inline size_t getWidth()
{
    if (isAVX512())
    {
        return getAVX512Width<T>();
    }
    if (isAVX())
    {
        return getAVXWidth<T>();
    }
    if (isSSE2())
    {
        return getSSE2Width<T>();
    }
    
    throw std::runtime_error("Must have at least SSE2.");
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
template <typename T, typename TFuncSSE2, typename TFuncAVX, typename TFuncAVX512>
inline auto getFuncForWidth(TFuncSSE2 fSSE2, TFuncAVX fAVX, TFuncAVX512 fAVX512)
{
    // At runtime, once we know we have SSE2/AVX/AVX512, that won't change.
    static const auto width = getWidth<T>();

    static const auto width_none = [](coda_oss::span<const T>, coda_oss::span<T>) {
        throw std::logic_error("Unknown 'width' value.");
    };

    return width == getSSE2Width<T>() ? fSSE2 :
        (width ==  getAVXWidth<T>()  ? fAVX :
            (width == getAVX512Width<T>() ? fAVX512 : width_none));
}

template<typename T>
inline auto getSin()
{
    static const auto simd_sin = [&](const auto& v) { return sin(v); };
    static const auto std_sin = [&](const auto& v) { return sin(v); };

    static const auto fSSE2 = [](coda_oss::span<const T> inputs, coda_oss::span<T> outputs) {
        return call_Funcs<getSSE2Width<T>()>(inputs, outputs,  simd_sin, std_sin);
    };
    static const auto fAVX = [](coda_oss::span<const T> inputs, coda_oss::span<T> outputs) {
        return  call_Funcs<getAVXWidth<T>()>(inputs, outputs, simd_sin, std_sin);
    };
    static const auto fAVX512 = [](coda_oss::span<const T> inputs, coda_oss::span<T> outputs) {
        return  call_Funcs<getAVX512Width<T>()>(inputs, outputs, simd_sin, std_sin);
    };
    return getFuncForWidth<T>(fSSE2, fAVX, fAVX512);
}
void simd::Sin(coda_oss::span<const float> inputs, coda_oss::span<float> outputs)
{
    static const auto f = getSin<float>();
    f(inputs, outputs); 
}
void simd::Sin(coda_oss::span<const double> inputs, coda_oss::span<double> outputs)
{
    static const auto f = getSin<double>();
    f(inputs, outputs); 
}
