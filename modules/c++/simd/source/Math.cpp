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

using InstructionSet = sys::SIMDInstructionSet;

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
template <typename T, InstructionSet> constexpr size_t Elements_per_type();
template <> constexpr size_t Elements_per_type<float, InstructionSet::SSE2>() { return 4; }
template <> constexpr size_t Elements_per_type<double, InstructionSet::SSE2>() { return 2; }
template <> constexpr size_t Elements_per_type<float, InstructionSet::AVX2>() { return 8; }
template <> constexpr size_t Elements_per_type<double, InstructionSet::AVX2>() { return 4; }
template <> constexpr size_t Elements_per_type<float, InstructionSet::AVX512F>() { return 16; }
template <> constexpr size_t Elements_per_type<double, InstructionSet::AVX512F>() { return 8; }

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
//template <> constexpr size_t Elements_per_type<std::complex<float>, InstructionSet::SSE2>() { return 1; }
template <> constexpr size_t Elements_per_type<std::complex<float>, InstructionSet::SSE2>() { return 2; }
template <> constexpr size_t Elements_per_type<std::complex<float>, InstructionSet::AVX2>() { return 4; }
template <> constexpr size_t Elements_per_type<std::complex<float>, InstructionSet::AVX512F>() { return 8; }
template <> constexpr size_t Elements_per_type<std::complex<double>, InstructionSet::SSE2>() { return 1; }
template <> constexpr size_t Elements_per_type<std::complex<double>, InstructionSet::AVX2>() { return 2; }
template <> constexpr size_t Elements_per_type<std::complex<double>, InstructionSet::AVX512F>() { return 4; }

template <typename T, typename U>
static void validate_inputs(span<const T> inputs, span<U> outputs)
{
    if (outputs.size() < inputs.size())
    {
        throw std::invalid_argument("'outputs' is smaller than 'inputs'");
    }
}
template <typename T1, typename U, typename T2>
static void validate_inputs(span<const T1> x_values, span<const T2> y_values, span<U> outputs)
{
    if (x_values.size() != y_values.size())
    {
        throw std::invalid_argument("inputs aren't the same size");
    }
    validate_inputs(x_values, outputs);
}

// Convert `instruction_set` to a width for `simd::Vec_t`.
template <InstructionSet instruction_set, typename T>
using simdVec = simd::Vec_t<Elements_per_type<T, instruction_set>(), T>; // e.g., vcl::Vec8f
// Convert `instruction_set` to a width for `simd::Complex_t`.
template <InstructionSet instruction_set, typename T>
using simdComplex = simd::Complex_t<Elements_per_type<std::complex<T>, instruction_set>(), T>; // e.g., vcl::Complex2f


// Decide between `simdVec` and `simdComplex`
template <InstructionSet instruction_set, typename T>
struct simdType final
{
    using type =  simdVec<instruction_set, T> /*vcl::Vec8f*/;
};
template <InstructionSet instruction_set, typename T>
struct simdType<instruction_set, std::complex<T>> final
{
    using type = simdComplex<instruction_set, T> /*vcl::Complex2f*/;
};
template <InstructionSet instruction_set, typename T>
using simdType_t = typename simdType<instruction_set, T>::type;

// Repeatedly load the appropriate `Vec`s with the inputs (`y_values` may
// be empty) and call the given function `f` (which will end up in SIMD code!).
// The results are stored in `outputs`.
// 
// This the actual workhorse function where most of the "interesting" stuff
// happens; much of the other code is "just" type manipulation.
template <typename TFunc,
    typename TXValues, typename TSimdX,
    typename TYValues, typename TSimdY, typename TOutputs>
inline void simd_Func(const TXValues& x_values, TSimdX& x,
    const TYValues& y_values, TSimdY& y,
    TOutputs& outputs, TFunc f)
{
    validate_inputs(x_values, y_values, outputs);

    // Do the check for an empty `y_values` just once: outside the loop.
    const std::function<void(size_t)> do_nothing = [&](size_t) {
        assert(y_values.empty());
    };
    const std::function<void(size_t)> load_y = [&](size_t i) {
        simd::load(y, y_values, i);  // load_a() requires very strict alignment
    };
    const auto maybe_load_y = y_values.empty() ? do_nothing : load_y;

    size_t i = 0;
    constexpr auto width = x.size();
    const auto size = x_values.size() <= width ? 0 : x_values.size() - width;  // don't walk off end with `+= width`
    for (; i < size; i += width)
    {
        simd::load(x, x_values, i);
        maybe_load_y(i);

        const auto results = f(x, y);

        simd::store(results, outputs, i);
    }

    // Finish whatever is left with load_partial() and store_partial()
    const auto remaining = gsl::narrow<int>(x_values.size() - i);
    simd::load_partial(x, remaining, x_values, i);
    if (!y_values.empty())
    {
        simd::load_partial(y, remaining, y_values, i);
    }
    const auto results = f(x, y);
    simd::store_partial(results, remaining, outputs, i);
}

template <typename TFunc,
    typename TInputs, typename TSimd, typename TOutputs>
inline void simd_Func(const TInputs& inputs, TSimd& v, TOutputs& outputs, TFunc f)
{
    validate_inputs(inputs, outputs);

    size_t i = 0;
    constexpr auto width = v.size();
    const auto size = inputs.size() <= width ? 0 : inputs.size() - width;  // don't walk off end with `+= width`
    for (; i < size; i += width)
    {
        simd::load(v, inputs, i);

        const auto results = f(v);

        simd::store(results, outputs, i);
    }

    // Finish whatever is left with load_partial() and store_partial()
    const auto remaining = gsl::narrow<int>(inputs.size() - i);
    simd::load_partial(v, remaining, inputs, i);
    const auto results = f(v);
    simd::store_partial(results, remaining, outputs, i);
}

template <size_t width, typename TFunc,
    typename T1, typename U = T1, typename T2 = T1>
inline void vec_Func(span<const T1> x_values, span<const T2> y_values, span<U> outputs, TFunc f)
{
    simd::Vec_t<width, T1> x{};  // e.g., vcl::Vec8f
    simd::Vec_t<width, T2> y{};  // e.g., vcl::Vec8f
    simd_Func(x_values, x, y_values, y, outputs, f);
}
template <size_t width, typename TFunc,
    typename T1, typename U = T1>
inline void vec_Func(span<const T1> inputs, span<U> outputs, TFunc f)
{
    simd::Vec_t<width, T1> v{};  // e.g., vcl::Vec8f
    simd_Func(inputs, v, outputs, f);
}

template <size_t width, typename TFunc,
    typename T, typename U = T>
inline void complex_Func(span<const std::complex<T>> inputs, span<U> outputs, TFunc f)
{
    simd::Complex_t<width, T> v{};  // e.g., vcl::Complex8f
    simd_Func(inputs, v, outputs, f);
}

// "bind" the compile-time `width` to an instantiation of simd_Func().
template <InstructionSet instruction_set, typename T1, typename T2, typename U, typename TFunc>
inline auto bind_vec2(TFunc f)
{
    return [&](span<const T1> x_values, span<const T2> y_values, span<U> outputs) {
        // For vector operations, the widths of all elements must be the same;
        // otherwise, it's not possible to walk through the `span`s.
        constexpr auto width = Elements_per_type<T1, instruction_set>();
        return vec_Func<width>(x_values, y_values, outputs, f); // e.g., vec_Func<4>(inputs, outputs, f)
    };
}
template<typename T1, typename TFunc, typename U = T1, typename T2 = T1>
inline void invoke_vec2(span<const T1> x_values, span<const T2> y_values, span<U> outputs, TFunc f)
{
    // At runtime, once we know we have SSE2/AVX/AVX512, that won't change.
    static const auto instruction_set = sys::OS().getSIMDInstructionSet();

    // For the given type and width, return the right instantiation of vec_Func.
    //
    // Each lambda is a different type even though they have the same signature.
    // Because of that, `auto` doesn't work since the inferred types are different.
    // The fix is to explicitly use std::function.
    using retval_t = std::function<void(span<const T1>, span<const T2>, span<U>)>;
    static const auto get_simd_func = [&f]() ->  retval_t {
        switch (instruction_set)
        {
        case InstructionSet::SSE2: return bind_vec2<InstructionSet::SSE2, T1, T2, U>(f);
        case InstructionSet::AVX2: return bind_vec2<InstructionSet::AVX2, T1, T2, U>(f);
        case InstructionSet::AVX512F: return bind_vec2<InstructionSet::AVX512F, T1, T2, U>(f);
        default:  break;
        }
        throw std::logic_error("Unknown 'instruction_set' value.");
    };

    // Only need to get the actual function once because the width won't change.
    static const auto func = get_simd_func();
    func(x_values, y_values, outputs);
}

// "bind" the compile-time `width` to an instantiation of simd_Func().
template <InstructionSet instruction_set, typename T, typename U, typename TFunc>
inline auto bind_vec(TFunc f)
{
    return [&](span<const T> inputs, span<U> outputs) {
        // For vector operations, the widths of all elements must be the same;
        // otherwise, it's not possible to walk through the `span`s.
        constexpr auto width = Elements_per_type<T, instruction_set>();
        return vec_Func<width>(inputs, outputs, f); // e.g., vec_Func<4>(inputs, outputs, f)
    };
}
template<typename T, typename TFunc, typename U = T>
inline void invoke_vec(span<const T> inputs, span<U> outputs, TFunc f)
{
    // At runtime, once we know we have SSE2/AVX/AVX512, that won't change.
    static const auto instruction_set = sys::OS().getSIMDInstructionSet();

    // For the given type and width, return the right instantiation of vec_Func.
    //
    // Each lambda is a different type even though they have the same signature.
    // Because of that, `auto` doesn't work since the inferred types are different.
    // The fix is to explicitly use std::function.
    using retval_t = std::function<void(span<const T>, span<U>)>;
    static const auto get_simd_func = [&f]() ->  retval_t {
        switch (instruction_set)
        {
        case InstructionSet::SSE2: return bind_vec<InstructionSet::SSE2, T, U>(f);
        case InstructionSet::AVX2: return bind_vec<InstructionSet::AVX2, T, U>(f);
        case InstructionSet::AVX512F: return bind_vec<InstructionSet::AVX512F, T, U>(f);
        default:  break;
        }
        throw std::logic_error("Unknown 'instruction_set' value.");
    };

    // Only need to get the actual function once because the width won't change.
    static const auto func = get_simd_func();
    func(inputs, outputs);
}

// "bind" the compile-time `width` to an instantiation of vec_Func().
template <InstructionSet instruction_set, typename T, typename U, typename TFunc>
inline auto bind_complex(TFunc f)
{
    return [&](span<const std::complex<T>> x_values, span<U> outputs) {
        // For vector operations, the widths of all elements must be the same;
        // otherwise, it's not possible to walk through the `span`s.
        constexpr auto width = Elements_per_type<std::complex<T>, instruction_set>();
        return complex_Func<width>(x_values, outputs, f); // e.g., vec_Func<4>(inputs, outputs, f)
    };
}
template<typename T, typename TFunc, typename U = T>
inline void invoke_complex(span<const std::complex<T>> x_values, span<U> outputs, TFunc f)
{
    // At runtime, once we know we have SSE2/AVX/AVX512, that won't change.
    static const auto instruction_set = sys::OS().getSIMDInstructionSet();

    // For the given type and width, return the right instantiation of vec_Func.
    //
    // Each lambda is a different type even though they have the same signature.
    // Because of that, `auto` doesn't work since the inferred types are different.
    // The fix is to explicitly use std::function.
    using retval_t = std::function<void(span<const std::complex<T>>, span<U>)>;
    static const auto get_simd_func = [&f]() ->  retval_t {
        switch (instruction_set)
        {
        case InstructionSet::SSE2: return bind_complex<InstructionSet::SSE2, T, U>(f);
        case InstructionSet::AVX2: return bind_complex<InstructionSet::AVX2, T, U>(f);
        case InstructionSet::AVX512F: return bind_complex<InstructionSet::AVX512F, T, U>(f);
        default:  break;
        }
        throw std::logic_error("Unknown 'instruction_set' value.");
    };

    // Only need to get the actual function once because the width won't change.
    static const auto func = get_simd_func();
    func(x_values, outputs);
}

template<typename T1, typename TFunc, typename U = T1, typename T2 = T1>
inline void invoke(span<const T1> x_values, span<const T2> y_values, span<U> outputs, TFunc f)
{
    invoke_vec2(x_values, y_values, outputs, f);
}
template <typename T, typename TFunc, typename U = T>
inline void invoke(span<const T> inputs, span<U> outputs, TFunc f)
{
    invoke_vec(inputs, outputs, f);
}

template <typename T, typename TFunc, typename U = T>
inline void invoke(span<const std::complex<T>> inputs, span<U> outputs, TFunc f)
{
    invoke_complex(inputs, outputs, f);
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

void simd::Arg(span<const std::complex<float>> zValues, span<float> outputs)
{
    static const auto f = [](const auto& zvec) {
        // https://en.cppreference.com/w/cpp/numeric/complex/arg
        // "... as if the function is implemented as std::atan2(std::imag(z), std::real(z))."
        return atan2(zvec.imag(), zvec.real());
    };
    invoke(zValues, outputs, f);
}
