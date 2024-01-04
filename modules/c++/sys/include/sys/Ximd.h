/* =========================================================================
 * This file is part of sys-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * © Copyright 2024, Maxar Technologies, Inc.
 *
 * sys-c++ is free software; you can redistribute it and/or modify
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

/*!
 *  \brief A simple ("poor man's) "implemtation" of std::experimental::simd
 * for development/testing purposes.
 *
 * `std::experimental::simd` needs G++11 and C++20 and the "vectorclass"
 * library needs C++17.  "Rolling our own" using `std::array` lets developers
 * try out some things without needing actual SIMD code.
 *
 */

#include "Dbg.h"

// This is intended for development/testing purposes, so enable by default only for debug.
#ifndef CODA_OSS_Ximd_ENABLED
#define CODA_OSS_Ximd_ENABLED CODA_OSS_DEBUG
#endif

#if CODA_OSS_Ximd_ENABLED

#include <array>

// By default, we'll put things in the `sys` namespace as everything
// else here does; but a developer might want to change that.
#ifndef CODA_OSS_Ximd_namespace
#define CODA_OSS_Ximd_namespace sys
#endif
namespace CODA_OSS_Ximd_namespace
{
constexpr size_t native_size = 4;

// Need a class for the "broadcast" constructor.
// Also helps to avoid overloading `std::array`.
template <typename T>
struct Ximd final
{
    using value_type = T;
    using reference = T&;

    Ximd() = default;
    Ximd(const value_type& v) noexcept
    {
        *this = Ximd([&](size_t) { return v; });
    }
    template <typename U>
    Ximd(const Ximd<U>& other) noexcept
    {
        *this = other;
    }

    // https://en.cppreference.com/w/cpp/experimental/simd/simd/simd
    template <typename G>
    explicit Ximd(G&& generator) noexcept
    {
        for (size_t i = 0; i < value.size(); i++)
        {
            value[i] = generator(i);
        }
    }

    reference operator[](size_t pos) noexcept
    {
        return value[pos];
    }
    value_type operator[](size_t pos) const noexcept
    {
        return value[pos];
    }

    constexpr auto size() const noexcept
    {
        return value.size();
    }

    Ximd& operator++() noexcept
    {
        *this = Ximd([&](size_t i) { return ++value[i]; });
        return *this;
    }
    Ximd operator++(int) noexcept
    {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }

 private:
    std::array<value_type, native_size> value{};
};

using ximd_mask = Ximd<bool>;

template <typename T>
inline auto operator/(const Ximd<T>& lhs, const Ximd<T>& rhs) noexcept
{
    return Ximd<T>([&](size_t i) { return lhs[i] / rhs[i]; });
}

template <typename T>
inline auto operator*(const Ximd<T>& lhs, const Ximd<T>& rhs) noexcept
{
    return Ximd<T>([&](size_t i) { return lhs[i] * rhs[i]; });
}

template <typename T>
inline auto operator-(const Ximd<T>& lhs, const Ximd<T>& rhs)
{
    return Ximd([&](size_t i) { return lhs[i] - rhs[i]; });
}
template <typename T>
inline auto& operator-=(Ximd<T>& lhs, const Ximd<T>& rhs)
{
    lhs = Ximd<T>([&](size_t i) {
                lhs[i] -= rhs[i];
                return lhs[i];
            });
    return lhs;
}

template <typename T>
inline auto& operator+=(Ximd<T>& lhs, const Ximd<T>& rhs)
{
    lhs = Ximd<T>([&](size_t i) {
                lhs[i] += rhs[i];
                return lhs[i];
            });
    return lhs;
}

template <typename T>
inline auto operator+(const Ximd<T>& lhs, const Ximd<T>& rhs)
{
    return Ximd<T>([&](size_t i) { return lhs[i] + rhs[i]; });
}

template <typename T>
inline auto operator==(const Ximd<T>& lhs, const Ximd<T>& rhs)
{
    return ximd_mask([&](size_t i) { return lhs[i] == rhs[i]; });
}
template <typename T>
inline auto operator<(const Ximd<T>& lhs, const Ximd<T>& rhs)
{
    return ximd_mask([&](size_t i) { return lhs[i] < rhs[i]; });
}
template <typename T>
inline auto operator<=(const Ximd<T>& lhs, const Ximd<T>& rhs)
{
    return ximd_mask([&](size_t i) { return lhs[i] <= rhs[i]; });
}

inline bool any_of(const ximd_mask& m)
{
    for (size_t i = 0; i < m.size(); i++)
    {
        if (m[i])
        {
            return true;
        }
    }
    return false;
}

} // CODA_OSS_Ximd_namespace

#endif