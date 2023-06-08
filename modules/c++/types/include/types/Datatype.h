/* =========================================================================
 * This file is part of types-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * (C) Copyright 2023, Maxar Technologies, Inc.
 *
 * types-c++ is free software; you can redistribute it and/or modify
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

#pragma once
#ifndef CODA_OSS_types_Datatype_h_INCLUDED_
#define CODA_OSS_types_Datatype_h_INCLUDED_

#include <complex>
#include <utility>
#include <stdexcept>
#include "coda_oss/cstddef.h"
#include "complex_short.h"

namespace types
{
namespace details
{
/**
 * The C Type \<==> MPI Type conversion functionality exists as converting a
 * variable of MPI_Datatype to an int or an int to a variable of MPI_Datatype
 * coincidentally works in a 32 bit environment.  The MPI_Datatype is internally
 * implemented as a pointer. This is needed for the serializing/deserializing
 * operations which need to pass a variable of MPI_Datatype using
 * MPI_Send/MPI_Recv/MPI_Bcast operations.
 *
 * Details on the lengths of the corresponding MPI_Datatype are covered in
 * table 13.2 "external32" sizes of predefined datatypes in MPI: A
 * Message-Passing Interface Standard Version 3.0  (p.536) dated: September 21,
 * 2012
 *
 * Note: The numbers used below for the constant ints must be unique and fit
 * within an integer. The list below is not exhaustive, but does contain values
 * for the types commonly used. Refer to the above table before adding more
 * types.
 */
constexpr int C_TYPE_BYTE = 101;
constexpr int C_TYPE_CHAR = 102;
constexpr int C_TYPE_SHORT = 103;
constexpr int C_TYPE_INT = 104;
constexpr int C_TYPE_FLOAT = 105;
constexpr int C_TYPE_LONG = 106;
constexpr int C_TYPE_LONG_LONG = 107;
constexpr int C_TYPE_DOUBLE = 108;
constexpr int C_TYPE_LONG_DOUBLE = 109;
constexpr int C_TYPE_COMPLEX = 110;
constexpr int C_TYPE_DOUBLE_COMPLEX = 111;
constexpr int C_TYPE_UNSIGNED_CHAR = 112;
constexpr int C_TYPE_UNSIGNED_SHORT = 113;
constexpr int C_TYPE_UNSIGNED_INT = 114;

constexpr int C_TYPE_SHORT_COMPLEX = 115; // types::complex_short; std::complex<short> is no longer valid in C++23
constexpr int C_TYPE_LONG_DOUBLE_COMPLEX = 116; // std::complex<long double>
constexpr int C_TYPE_UNSIGNED_LONG = 117;
constexpr int C_TYPE_UNSIGNED_LONG_LONG = 118;
}

// It can be convenient to have a "runtime" datatype; for example,
// it allows "generic" code to be written without templates in .H files.
//
// In any case, other projects already to this; so have it someplace common.
enum class Datatype
{
    Byte = details::C_TYPE_BYTE,

    Char = details::C_TYPE_CHAR,
    Short = details::C_TYPE_SHORT,
    Int = details::C_TYPE_INT,
    Long = details::C_TYPE_LONG,
    LongLong = details::C_TYPE_LONG_LONG,

    Float = details::C_TYPE_FLOAT,
    Double = details::C_TYPE_DOUBLE,
    LongDouble = details::C_TYPE_LONG_DOUBLE,

    Complex = details::C_TYPE_COMPLEX,
    FloatComplex = Complex,
    DoubleComplex = details::C_TYPE_DOUBLE_COMPLEX,
    LongDoubleComplex = details::C_TYPE_LONG_DOUBLE_COMPLEX,
    ShortComplex = details::C_TYPE_SHORT_COMPLEX,

    UnsignedChar = details::C_TYPE_UNSIGNED_CHAR,
    UnsignedShort = details::C_TYPE_UNSIGNED_SHORT,
    UnsignedInt = details::C_TYPE_UNSIGNED_INT,
    UnsignedLong = details::C_TYPE_UNSIGNED_LONG,
    UnsignedLongLong = details::C_TYPE_UNSIGNED_LONG_LONG,
};

/*
* Gets the Datatype associated with the templated type.
*
* @return Datatype
*/
template<typename T> constexpr Datatype getDatatype();
template<> constexpr Datatype getDatatype<coda_oss::byte>() { return Datatype::Byte; }
template<> constexpr Datatype getDatatype<char>() { return Datatype::Char; }
template<> constexpr Datatype getDatatype<short>() { return Datatype::Short; }
template<> constexpr Datatype getDatatype<int>() { return Datatype::Int; }
template<> constexpr Datatype getDatatype<long>() { return Datatype::Long; }
template<> constexpr Datatype getDatatype<long long>() { return Datatype::LongLong; }

template<> constexpr Datatype getDatatype<float>() { return Datatype::Float; }
template<> constexpr Datatype getDatatype<double>() { return Datatype::Double; }
template<> constexpr Datatype getDatatype<long double>() { return Datatype::LongDouble; }

template<> constexpr Datatype getDatatype<complex_short>() { return Datatype::ShortComplex; }
template<> constexpr Datatype getDatatype<std::complex<float>>() { return Datatype::FloatComplex; }
template<> constexpr Datatype getDatatype<std::complex<double>>() { return Datatype::DoubleComplex; }
template<> constexpr Datatype getDatatype<std::complex<long double>>() { return Datatype::LongDoubleComplex; }

template<> constexpr Datatype getDatatype<unsigned char>() { return Datatype::UnsignedChar; }
template<> constexpr Datatype getDatatype<unsigned short>() { return Datatype::UnsignedShort; }
template<> constexpr Datatype getDatatype<unsigned int>() { return Datatype::UnsignedInt; }
template<> constexpr Datatype getDatatype<unsigned long>() { return Datatype::UnsignedLong; }
template<> constexpr Datatype getDatatype<unsigned long long>() { return Datatype::UnsignedLongLong; }

// Hold a type such a `int` and it's Datatype together
template <typename T> struct Datatype_TagT final
{
    using type = T; // e.g., `int`
    static constexpr Datatype value() { return getDatatype<T>(); } // Datatype::Int
};
template <typename T> using Datatype_Tag = typename T::type;

// Utility routine to make it easier to call template'd function using a Datatype.
//    template<typename T> void foo() { ... }
//    void process(Datatype t) {
//        const auto f = [&](auto tag) { foo<types::Datatype_Tag<decltype(tag)>(); };
//        types::invoke(type, f);
//    }
template <typename T> using Datatype_Tag = typename T::type;
template <typename F, typename ...Args>
inline decltype(auto) invoke(Datatype t, F&& func, Args&& ...args) {
if (t == Datatype::Byte) return std::forward<F>(func)(Datatype_TagT<coda_oss::byte>{}, std::forward<Args>(args)...);
if (t == Datatype::Char) return std::forward<F>(func)(Datatype_TagT<char>{}, std::forward<Args>(args)...);
if (t == Datatype::Short) return std::forward<F>(func)(Datatype_TagT<short>{}, std::forward<Args>(args)...);
if (t == Datatype::Int) return std::forward<F>(func)(Datatype_TagT<int>{}, std::forward<Args>(args)...);
if (t == Datatype::Long) return std::forward<F>(func)(Datatype_TagT<long>{}, std::forward<Args>(args)...);
if (t == Datatype::LongLong) return std::forward<F>(func)(Datatype_TagT<long long>{}, std::forward<Args>(args)...);

if (t == Datatype::Float) return std::forward<F>(func)(Datatype_TagT<float>{}, std::forward<Args>(args)...);
if (t == Datatype::Double) return std::forward<F>(func)(Datatype_TagT<double>{}, std::forward<Args>(args)...);
if (t == Datatype::LongDouble) return std::forward<F>(func)(Datatype_TagT<long double>{}, std::forward<Args>(args)...);

if (t == Datatype::ShortComplex) return std::forward<F>(func)(Datatype_TagT<complex_short>{}, std::forward<Args>(args)...);
if (t == Datatype::FloatComplex) return std::forward<F>(func)(Datatype_TagT<std::complex<float>>{}, std::forward<Args>(args)...);
if (t == Datatype::DoubleComplex) return std::forward<F>(func)(Datatype_TagT<std::complex<double>>{}, std::forward<Args>(args)...);
if (t == Datatype::LongDoubleComplex) return std::forward<F>(func)(Datatype_TagT<std::complex<long double>>{}, std::forward<Args>(args)...);

if (t == Datatype::UnsignedChar) return std::forward<F>(func)(Datatype_TagT<unsigned char>{}, std::forward<Args>(args)...);
if (t == Datatype::UnsignedShort) return std::forward<F>(func)(Datatype_TagT<unsigned short>{}, std::forward<Args>(args)...);
if (t == Datatype::UnsignedInt) return std::forward<F>(func)(Datatype_TagT<unsigned int>{}, std::forward<Args>(args)...);
if (t == Datatype::UnsignedLong) return std::forward<F>(func)(Datatype_TagT<unsigned long>{}, std::forward<Args>(args)...);
if (t == Datatype::UnsignedLongLong) return std::forward<F>(func)(Datatype_TagT<unsigned long long>{}, std::forward<Args>(args)...);

throw std::invalid_argument("Unexpected 'Datatype'");
}

}

#endif  // CODA_OSS_types_Datatype_h_INCLUDED_
