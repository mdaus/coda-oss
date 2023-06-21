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
inline size_t getSSE2Width();
template <typename T>
inline size_t getAVXWidth();
template <typename T>
inline size_t getAVX512Width();

template<> static inline size_t getSSE2Width<float>()
{
    return isSSE2() ? 4 : 0;
}
template <> static inline size_t getSSE2Width<double>()
{
    return isSSE2() ? 2 : 0;
}

template<> static inline size_t getAVXWidth<float>()
{
    return isAVX() ? 8 : 0;
}
template <> static inline size_t getAVXWidth<double>()
{
    return isAVX() ? 4 : 0;
}

template<> static inline size_t getAVX512Width<float>()
{
    return isAVX() ? 16 : 0;
}
template <> static inline size_t getAVX512Width<double>()
{
    return isAVX() ? 8 : 0;
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

template <size_t width, typename T>
inline void Sin_(coda_oss::span<const T> inputs, coda_oss::span<T> outputs)
{
    if (outputs.size() < inputs.size())
    {
        throw std::invalid_argument("'outputs' smaller than 'inputs'");
    }

    simd::Vec<T, width> vec;  // i.e., vcl::Vec8f

    #ifndef NDEBUG // i.e., debug, not release
    // The output could be bigger than input; help identify walking off the end.
    for (auto& output : outputs)
    {
        output = std::numeric_limits<T>::min();;
    }
    #endif

    const auto inputs_size = inputs.size() < width ? 0 : inputs.size() - width;  // don't walk off end with `+= width`
    size_t i = 0;
    for (; i < inputs_size; i += width)
    {
        auto const pInputs = &(inputs[i]);
        vec.load(pInputs);  // load_a() requires very strict alignment

        const auto results = sin(vec);

        auto const pOutputs = &(outputs[i]);
        results.store(pOutputs);  // store_a() requires very strict alignment
    }

    // Finish any remaining one at a time
    for (; i < inputs.size(); i++)
    {
        outputs[i] = sin(inputs[i]);
    }
}

void simd::Sin(coda_oss::span<const float> inputs, coda_oss::span<float> outputs)
{
    static const auto width_4 = [](coda_oss::span<const float> inputs, coda_oss::span<float> outputs) {
        return  Sin_<4>(inputs, outputs);
    };
    static const auto width_8 = [](coda_oss::span<const float> inputs, coda_oss::span<float> outputs) {
        return  Sin_<8>(inputs, outputs);
    };
    static const auto width_16 = [](coda_oss::span<const float> inputs, coda_oss::span<float> outputs) {
        return  Sin_<16>(inputs, outputs);
    };
    static const auto width_none = [](coda_oss::span<const float>, coda_oss::span<float>) {
        throw std::logic_error("Unknown 'width' value.");
    };

    // At runtime, once we know we have SSE2/AVX/AVX512, that won't change.
    static const auto width = getWidth<float>();
    static const auto f = width == 4 ? width_4 :
            (width == 8 ? width_8 :
                (width == 16 ? width_16 : width_none));
    // this should be a fast call (all lambdas) to a specific instantiation of Sin_<N>
    f(inputs, outputs); 
}
void simd::Sin(coda_oss::span<const double> inputs, coda_oss::span<double> outputs)
{
    static const auto width_2 = [](coda_oss::span<const double> inputs, coda_oss::span<double> outputs) {
        return  Sin_<2>(inputs, outputs);
    };
    static const auto width_4 = [](coda_oss::span<const double> inputs, coda_oss::span<double> outputs) {
        return  Sin_<4>(inputs, outputs);
    };
    static const auto width_8 = [](coda_oss::span<const double> inputs, coda_oss::span<double> outputs) {
        return  Sin_<8>(inputs, outputs);
    };
    static const auto width_none = [](coda_oss::span<const double>, coda_oss::span<double>) {
        throw std::logic_error("Unknown 'width' value.");
    };

    // At runtime, once we know we have SSE2/AVX/AVX512, that won't change.
    static const auto width = getWidth<double>();
    static const auto f = width == 2 ? width_2 :
            (width == 4 ? width_4 :
                (width == 8 ? width_8 : width_none));
    // this should be a fast call (all lambdas) to a specific instantiation of Sin_<N>
    f(inputs, outputs); 
}
