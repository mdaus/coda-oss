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

#if _MSC_VER
// TODO: get rid of #pragmas
#pragma warning(disable: 4866) // compiler may not enforce left-to-right evaluation order for call to '...'
#pragma warning(disable: 6323) // Use of arithmetic operator on Boolean type(s).
#pragma warning(disable: 26818) // Switch statement does not cover all cases. Consider adding a '...' label (es.79).
#pragma warning(disable: 26475) // Do not use function style casts (es.49). Prefer '...' over '...'..
#pragma warning(disable: 26497) // You can attempt to make '...' constexpr unless it contains any undefined behavior (f.4).
#pragma warning(disable: 26477) // Use '...' rather than 0 or NULL (es.47).
#pragma warning(disable: 6001) // Using uninitialized memory '...'.
#pragma warning(disable: 6297) // Arithmetic overflow.  Results might not be an expected value.
#pragma warning(disable: 6316) // Incorrect operator.  Use bitwise-and to determine whether bits are set.
#pragma warning(disable: 26444) // Don't try to declare a local variable with no name (es.84).
#pragma warning(disable: 6201) // Index '...' is out of valid index range '...' to '...' for possibly stack allocated buffer '...'.
#endif

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

#include "vectorclass/version2/instrset_detect.cpp" // TODO

// No "using namespace simd", do this instead to reduce a bit of noise.
template<typename T>
using span = simd::span<T>;

using InstructionSet = sys::SIMDInstructionSet;

// From instrset_detect(); see the table in Section 9.9 of the documentation.
// https://github.com/vectorclass/manual/raw/master/vcl_manual.pdf
enum class instrset
{
    x86 = 0, //  80386 instruction set
    SSE = 1,
    SSE2 = 2,
    SSE3 = 3,
    SSSE3 = 4, // Supplementary SSE3
    SSE4_1 = 5,
    SSE4_2 = 6,
    AVX = 7,
    AVX2 = 8,
    AVX512F = 9,
    AVX512VL = 10,
    AVX512BW = 10,
    AVX512DQ = 10,
};
static auto instrset_detect()
{
    const auto result = vcl::instrset_detect();
    return static_cast<instrset>(result);
}

static auto instruction_set_()
{
    const auto instructionSet = sys::OS().getSIMDInstructionSet();
    const auto instrset = instrset_detect();
    switch (instructionSet)
    {
    case InstructionSet::SSE2:
        if (instrset < instrset::SSE2)
        {
            throw std::runtime_error("instruction set mismatch.");
        }
        return instructionSet;

    case InstructionSet::AVX2:
        if (instrset < instrset::AVX2)
        {
            throw std::runtime_error("instruction set mismatch.");
        }
        return instructionSet;

    case InstructionSet::AVX512F:
        if (instrset < instrset::AVX512F)
        {
            throw std::runtime_error("instruction set mismatch.");
        }
        return instructionSet;

    default: break;
    }
    throw std::logic_error("Unknown 'instruction_set' value.");
}
static auto instruction_set()
{
    // At runtime, once we know we have SSE2/AVX/AVX512, that won't change.
    static const auto retval = instruction_set_();
    return retval;
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
template <typename T1, typename T2, typename U>
static void validate_inputs(span<const T1> x_values, span<const T2> y_values, span<U> outputs)
{
    if (x_values.size() != y_values.size())
    {
        throw std::invalid_argument("inputs aren't the same size");
    }
    validate_inputs(x_values, outputs);
}
template <typename T, typename U1,  typename U2>
static void validate_inputs(span<const T> inputs, span<U1> outputs1, span<U2> outputs2)
{
    if (outputs1.size() != outputs2.size())
    {
        throw std::invalid_argument("outputs aren't the same size");
    }
    validate_inputs(inputs, outputs1);
}

// Decide between ` simd::Vec_t` and ` simd::Complex_t`
template <size_t width, typename T>
struct simdType final
{
    using type = simd::Vec_t<width, T> /*vcl::Vec8f*/;
};
template <size_t width, typename T>
//struct simdType<width, std::complex<T>> final
//{
//    using type = simd::Complex_t<width, T> /*vcl::Complex2f*/;
//};
template <size_t width, typename T>
using simdType_t = typename simdType<width, T>::type;

// Repeatedly load the appropriate `Vec`s with the inputs (`y_values` may
// be empty) and call the given function `f` (which will end up in SIMD code!).
// The results are stored in `outputs`.
// 
// This the actual workhorse function where most of the "interesting" stuff
// happens; much of the other code is "just" type manipulation.
template <size_t width, typename T1, typename TSpan2, typename U, typename TFunc>
inline void simd_Func_(span<const T1> x_values, TSpan2 y_values, span<U> outputs, TFunc f)
{
    using t1_t = typename decltype(x_values)::value_type; // T or std::complex<T>
    simdType_t<width, t1_t> x{};  // e.g., vcl::Vec8f

    using y_value_type= typename decltype(y_values)::value_type; // no `const`
    constexpr auto y_is_nullptr_t = std::is_same_v<nullptr_t, y_value_type>;
    using t2_t = std::conditional_t<y_is_nullptr_t, t1_t, y_value_type>; // need something other than `nullptr_t` for Vec<>
    simdType_t<width, t2_t> y{};  // e.g., vcl::Vec8f

    using y_element_type = typename decltype(y_values)::element_type; // value_type removes `const`
    constexpr auto y_is_input = y_is_nullptr_t ? false : std::is_const_v<y_element_type>;
    constexpr auto y_is_output = y_is_nullptr_t ? false : !y_is_input;

    const auto size = x_values.size() <= width ? 0 : x_values.size() - width;  // don't increment past `.size()` with `+= width`
    size_t i = 0;
    for (; i < size; i += width)
    {
        simd::load(x, x_values, i);
        if constexpr (y_is_input)
        {
            simd::load(y, y_values, i);
        }

        const auto results = f(x, y);

        simd::store(results, outputs, i);
        if constexpr (y_is_output)
        {
            simd::store(y, y_values, i);        
        }
    }

    // Finish whatever is left with load_partial() and store_partial()
    const auto remaining = gsl::narrow<int>(x_values.size() - i);
    if (remaining > 0)
    {
        simd::load_partial(x, remaining, x_values, i);
        if constexpr (y_is_input)
        {
            simd::load_partial(y, remaining, y_values, i);
        }
        const auto results = f(x, y);
        simd::store_partial(results, remaining, outputs, i);
        if constexpr (y_is_output)
        {
            simd::store_partial(y, remaining, y_values, i);
        }
    }
}
template <size_t width, typename T1, typename T2, typename U, typename TFunc>
inline void simd_Func(span<const T1> x_values, span<const T2> y_values, span<U> outputs, TFunc f)
{
    validate_inputs(x_values, y_values, outputs);
    simd_Func_<width>(x_values, y_values, outputs, f);
}

template <size_t width, typename T, typename U1, typename U2, typename TFunc>
inline void simd_Func(span<const T> inputs, span<U1> outputs1, span<U2> outputs2, TFunc f)
{
    validate_inputs(inputs, outputs1, outputs2);
    simd_Func_<width>(inputs, outputs1, outputs2, f);
}

template <size_t width, typename T, typename U, typename TFunc>
inline void simd_Func(span<const T> inputs, span<U> outputs, TFunc f)
{
    validate_inputs(inputs, outputs);

    static const span<const nullptr_t> empty;
    simd_Func_<width>(inputs, empty, outputs, f);
}

/***************************************************************************************************/

template<typename TRetval, typename TFuncSSE2, typename TFuncAVX2, typename TFuncAVX512F>
inline TRetval get_simd_func(TFuncSSE2 fSSE2, TFuncAVX2 fAVX2, TFuncAVX512F fAFX512f)
{
    switch (instruction_set())
    {
    case InstructionSet::SSE2: return fSSE2;
    case InstructionSet::AVX2: return fAVX2;
    case InstructionSet::AVX512F: return fAFX512f;
    default:  break;
    }
    throw std::logic_error("Unknown 'instruction_set' value.");
}

// "bind" the compile-time `width` to an instantiation of simd_Func().
template <InstructionSet instruction_set, typename T, typename U, typename TFunc>
inline auto bind_simd(TFunc f)
{
    return [&](span<const T> inputs, span<U> outputs) {
        // For vector operations, the widths of all elements must be the same;
        // otherwise, it's not possible to walk through the `span`s.
        constexpr auto width = Elements_per_type<T, instruction_set>();
        return simd_Func<width>(inputs, outputs, f); // e.g., vec_Func<4>(inputs, outputs, f)
    };
}

// "bind" the compile-time `width` to an instantiation of simd_Func().
template <InstructionSet instruction_set, typename T1, typename T2, typename U, typename TFunc>
inline auto bind_simd_iio(TFunc f)
{
    return [&](span<const T1> x_values, span<const T2> y_values, span<U> outputs) {
        // For vector operations, the widths of all elements must be the same;
        // otherwise, it's not possible to walk through the `span`s.
        constexpr auto width = Elements_per_type<T1, instruction_set>();
        return simd_Func<width>(x_values, y_values, outputs, f); // e.g., vec_Func<4>(inputs, outputs, f)
    };
}

// "bind" the compile-time `width` to an instantiation of simd_Func().
template <InstructionSet instruction_set, typename T, typename U1, typename U2, typename TFunc>
inline auto bind_simd_ioo(TFunc f)
{
    return [&](span<const T> inputs, span<U1> outputs1, span<U2> outputs2) {
        // For vector operations, the widths of all elements must be the same;
        // otherwise, it's not possible to walk through the `span`s.
        constexpr auto width = Elements_per_type<T, instruction_set>();
        return simd_Func<width>(inputs, outputs1, outputs2, f); // e.g., vec_Func<4>(inputs, outputs, f)
    };
}

/***************************************************************************************************/

template<typename T1, typename T2, typename U, typename TFunc>
inline void invoke(span<const T1> x_values, span<const T2> y_values, span<U> outputs, TFunc f)
{
    // For the given type and width, return the right instantiation of vec_Func.
    //
    // Each lambda is a different type even though they have the same signature.
    // Because of that, `auto` doesn't work since the inferred types are different.
    // The fix is to explicitly use std::function.
    using retval_t = std::function<void(span<const T1>, span<const T2>, span<U>)>;

    // Only need to get the actual function once because the width won't change.
    static const auto func = get_simd_func<retval_t>(
            bind_simd_iio<InstructionSet::SSE2, T1, T2, U>(f), 
            bind_simd_iio<InstructionSet::AVX2, T1, T2, U>(f),
            bind_simd_iio<InstructionSet::AVX512F, T1, T2, U>(f));
    func(x_values, y_values, outputs);
}

template<typename T, typename U1, typename U2, typename TFunc>
inline void invoke(span<const T> values, span<U1> outputs1, span<U2> outputs2, TFunc f)
{
    // For the given type and width, return the right instantiation of vec_Func.
    //
    // Each lambda is a different type even though they have the same signature.
    // Because of that, `auto` doesn't work since the inferred types are different.
    // The fix is to explicitly use std::function.
    using retval_t = std::function<void(span<const T>, span<U1>, span<U2>)>;

    // Only need to get the actual function once because the width won't change.
    static const auto func = get_simd_func<retval_t>(
            bind_simd_ioo<InstructionSet::SSE2, T, U1, U2>(f), 
            bind_simd_ioo<InstructionSet::AVX2, T, U1, U2>(f),
            bind_simd_ioo<InstructionSet::AVX512F, T, U1, U2>(f));
    func(values, outputs1, outputs2);
}

template<typename T, typename U, typename TFunc>
inline void invoke(span<const T> inputs, span<U> outputs, TFunc f)
{
    using value_type = typename decltype(inputs)::value_type; // T or std::complex<T>

    // For the given type and width, return the right instantiation of vec_Func.
    //
    // Each lambda is a different type even though they have the same signature.
    // Because of that, `auto` doesn't work since the inferred types are
    // different. The fix is to explicitly use std::function.
    using retval_t = std::function<void(span<const value_type>, span<U>)>;

     // Only need to get the actual function once because the width won't change.
    static const auto func = get_simd_func<retval_t>(
            bind_simd<InstructionSet::SSE2, value_type, U>(f),
            bind_simd<InstructionSet::AVX2, value_type, U>(f),
            bind_simd<InstructionSet::AVX512F, value_type, U>(f));
    func(inputs, outputs);
}

/***************************************************************************************************/

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

void simd::SinCos(span<const float> inputs, span<float> sines, span<float> cosines)
{
    static const auto f = [](const auto& v, auto& v_cosines) { return sincos(&v_cosines, v); };
    invoke(inputs, cosines, sines, f);
}
void simd::SinCos(span<const double> inputs, span<double> sines, span<double> cosines)
{
    static const auto f = [](const auto& v, auto& v_cosines) { return sincos(&v_cosines, v); };
    invoke(inputs, cosines, sines, f);
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
//        // https://en.cppreference.com/w/cpp/numeric/complex/arg
//        // "... as if the function is implemented as std::atan2(std::imag(z), std::real(z))."
//        return atan2(zvec.imag(), zvec.real());
//    };
//    invoke(zValues, outputs, f);
//}
