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
#include <type_traits>

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
#include "Complex.h"

// No "using namespace simd", do this instead to reduce a bit of noise.
template<typename T>
using span = simd::span<T>;

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
template <typename T, sys::SIMDInstructionSet> constexpr size_t Elements_per_type();
template <> constexpr size_t Elements_per_type<float, sys::SIMDInstructionSet::SSE2>() { return 4; }
template <> constexpr size_t Elements_per_type<double, sys::SIMDInstructionSet::SSE2>() { return 2; }
template <> constexpr size_t Elements_per_type<float, sys::SIMDInstructionSet::AVX2>() { return 8; }
template <> constexpr size_t Elements_per_type<double, sys::SIMDInstructionSet::AVX2>() { return 4; }
template <> constexpr size_t Elements_per_type<float, sys::SIMDInstructionSet::AVX512F>() { return 16; }
template <> constexpr size_t Elements_per_type<double, sys::SIMDInstructionSet::AVX512F>() { return 8; }

/*
* Table 1.1 from https://github.com/vectorclass/manual/raw/master/vcl_manual.pdf
CxVector class    Precision   Elements per vector     Total bits      Minimum recommended instruction set
Complex1f	single          1 (2) 			128 		SSE2
Complex2f	single       	2 (4) 			128 		SSE2
Complex4f	single          4 (8) 			256 		AVX
Complex8f	single          8 (16) 			512 		AVX512
Complex1d	double          1 (2) 			128 		SSE2
Complex2d	double          2 (4) 			256 		AVX
Complex4d	double          4 (8) 			512 		AVX512
*/
//template <> constexpr size_t Elements_per_type<std::complex<float>, sys::SIMDInstructionSet::SSE2>() { return 1; }
template <> constexpr size_t Elements_per_type<std::complex<float>, sys::SIMDInstructionSet::SSE2>() { return 2; }
template <> constexpr size_t Elements_per_type<std::complex<float>, sys::SIMDInstructionSet::AVX2>() { return 4; }
template <> constexpr size_t Elements_per_type<std::complex<float>, sys::SIMDInstructionSet::AVX512F>() { return 8; }
template <> constexpr size_t Elements_per_type<std::complex<double>, sys::SIMDInstructionSet::SSE2>() { return 1; }
template <> constexpr size_t Elements_per_type<std::complex<double>, sys::SIMDInstructionSet::AVX2>() { return 2; }
template <> constexpr size_t Elements_per_type<std::complex<double>, sys::SIMDInstructionSet::AVX512F>() { return 4; }

// Gets the RUNTIME width for the given type and instruction-set; see table, above.
template<typename T>
inline size_t getWidth()
{
    static const auto instruction_set = sys::OS().getSIMDInstructionSet();
    switch (instruction_set)
    {
    case sys::SIMDInstructionSet::SSE2: return Elements_per_type<T, sys::SIMDInstructionSet::SSE2>();
    case sys::SIMDInstructionSet::AVX2: return Elements_per_type<T, sys::SIMDInstructionSet::AVX2>();
    case sys::SIMDInstructionSet::AVX512F: return Elements_per_type<T, sys::SIMDInstructionSet::AVX512F>();
    default: break;
    }
    throw std::logic_error("Unexpected sys::SIMDInstructionSet value.");
}

template <typename T1, typename U = T1, typename T2 = T1>
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


//template <size_t width, typename T>
//using simdType = std::conditional_t<std::is_arithmetic<T>::value, simd::Vec_t<width, T> /*vcl::Vec8f*/, simd::Complex_t<width, T> /*vcl::Complex2f*/>;
template <size_t width, typename T>
struct simdType final
{
    using type =  simd::Vec_t<width, T> /*vcl::Vec8f*/;
};
template <size_t width, typename T>
struct simdType<width, std::complex<T>> final
{
    using type = simd::Complex_t<width, T> /*vcl::Complex2f*/;
};
template <size_t width, typename T>
using simdType_t = typename simdType<width, T>::type;

template <size_t width, typename T>
inline void load(simd::Vec_t<width, T>& vec, span<const T> values, size_t i)
{
    vec.load(&(values[i]));  // load_a() requires very strict alignment
}
template <size_t width, typename T>
inline void load_partial(simd::Vec_t<width, T>& vec, int n, span<const T> values, size_t i)
{
    vec.load_partial(n, &(values[i]));
}
template <size_t width, typename T>
inline void store(const simd::Vec_t<width, T>& vec, span<T> results, size_t i)
{
    vec.store(&(results[i]));  // store_a() requires very strict alignment
}
template <size_t width, typename T>
inline void store_partial(const simd::Vec_t<width, T>& vec, int n, span<T> results, size_t i)
{
    vec.store_partial(n, &(results[i]));
}

template <size_t width, typename T, typename TValue = typename T::value_type>
inline void load(simd::Complex_t<width, TValue>& cx, span<const T> values, size_t i)
{
    const void* const pValues = &(values[i]);
    cx.load(static_cast<const TValue*>(pValues));
}
template <size_t width, typename T, typename TValue = typename T::value_type>
inline void load_partial(simd::Complex_t<width, TValue>& cx, int n, span<const T> values, size_t i)
{
    for (int j = 0; j < n; j++)
    {
        auto&& value = values[i + j];
        cx.insert(j, simd::Complex_t<1, TValue>(value.real(), value.imag()));    
    }
}
template <size_t width, typename T, typename TValue = typename T::value_type>
inline void store(const simd::Complex_t<width, TValue>& cx, span<T> results, size_t i)
{
    void* const pResults = &(results[i]);
    cx.store(static_cast<TValue*>(pResults));
}
template <size_t width, typename T, typename TValue = typename T::value_type>
inline void store_partial(const simd::Complex_t<width, TValue>& cx, int n, span<T> results_, size_t i)
{
    for (int j = 0; j < n; j++)
    {
        auto results = sys::make_span(results_.data() + j, 1);
        store(cx.extract(j), results, 0);
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

    simdType_t<width, T1> x{};  // e.g., vcl::Vec8f
    simdType_t<width, T2> y{};  // e.g., vcl::Vec8f

    // Do the check for an empty `y_values` just once: outside the loop.
    const std::function<void(size_t)> do_nothing = [&](size_t) {
        assert(y_values.empty());
    };
    const std::function<void(size_t)> load_y = [&](size_t i) {
        load<width>(y, y_values, i);  // load_a() requires very strict alignment
    };
    const auto maybe_load_y = y_values.empty() ? do_nothing : load_y;

    size_t i = 0;
    const auto size = x_values.size() <= width ? 0 : x_values.size() - width;  // don't walk off end with `+= width`
    for (; i < size; i += width)
    {
        load<width>(x, x_values, i);
        maybe_load_y(i);

        const auto results = f(x, y);

        store<width>(results, outputs, i);
    }

    // Finish whatever is left with load_partial() and store_partial()
    const auto remaining = gsl::narrow<int>(x_values.size() - i);
    load_partial<width>(x, remaining, x_values, i);
    if (!y_values.empty())
    {
        load_partial<width>(y, remaining, y_values, i);
    }
    const auto results = f(x, y);
    store_partial<width>(results, remaining, outputs, i);
}

// "bind" the compile-time `width` to an instantiation of vec_Func().
template <size_t width, typename T1, typename T2, typename U, typename TFunc>
inline auto bind(TFunc f)
{
    return [&](span<const T1> x_values, span<const T2> y_values, span<U> outputs) {
        return vec_Func<width>(x_values, y_values, outputs, f); // e.g., vec_Func<4>(inputs, outputs, f)
    };
}

template<typename T1, typename TFunc, typename U = T1, typename T2 = T1>
inline void invoke(span<const T1> x_values, span<const T2> y_values, span<U> outputs, TFunc f)
{
    // For the given type and width, return the right instantiation of vec_Func.
    //
    // Each lambda is a different type even though they have the same signature.
    // Because of that, `auto` doesn't work since the inferred types are different.
    // The fix is to explicitly use std::function.
    using retval_t = std::function<void(span<const T1>, span<const T2>, span<U>)>;
    static const auto get_simd_func = [&f]() ->  retval_t {
        constexpr auto sse2_width = Elements_per_type<T1,  sys::SIMDInstructionSet::SSE2>();
        constexpr auto avx2_width = Elements_per_type<T1,  sys::SIMDInstructionSet::AVX2>();
        constexpr auto avx512f_width = Elements_per_type<T1,  sys::SIMDInstructionSet::AVX512F>();

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

//void simd::Arg(span<const std::complex<float>> zValues, span<float> outputs)
//{
//    static const auto f = [](const auto& zvec, const auto&) {
//        const auto real = zvec.real();
//        const auto imag = zvec.imag();
//
//        // https://en.cppreference.com/w/cpp/numeric/complex/arg
//        // "... as if the function is implemented as std::atan2(std::imag(z), std::real(z))."
//        return atan2(imag, real);
//    };
//    invoke(zValues, outputs, f);
//}
