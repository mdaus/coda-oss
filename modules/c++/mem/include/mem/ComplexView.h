/* =========================================================================
 * This file is part of mem-c++
 * =========================================================================
 *
 * (C) Copyright 2013 - 2014, MDA Information Systems LLC
 * (C) Copyright 2023, Maxar Technologies, Inc.
 *
 * mem-c++ is free software; you can redistribute it and/or modify
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

#ifndef CODA_OSS_mem_ComplexView_h_INCLUDED_
#define CODA_OSS_mem_ComplexView_h_INCLUDED_
#pragma once

#include <assert.h>

#include <vector>
#include <complex>
#include <stdexcept>

#include "coda_oss/span.h"

namespace mem
{

 /*!
 *  \class ComplexView
 *  \brief These classes class provide read-only views onto a collection of complex
 * numbers. For the simple case, it's roughtly std::span<const std::complex<T>>. However,
 * sometimes the data is in two parallel arrays:
 *    const float* reals;
 *    const float* imags;
 * This classes have (about) the same access routines regardless of 
 * how the underlying data is actually stored.
 */

template <typename T>
struct ComplexArrayPtrView final
{
    using size_type = size_t;
    using value_type = std::complex<T>;

    ComplexArrayPtrView() = delete;
    ~ComplexArrayPtrView() = default;
    using span_t = coda_oss::span<const std::complex<T>>;
    explicit ComplexArrayPtrView(span_t data) : data_(data)
    {
    }
    ComplexArrayPtrView(const ComplexArrayPtrView&) = default;
    ComplexArrayPtrView& operator=(const ComplexArrayPtrView&) = default;
    ComplexArrayPtrView(ComplexArrayPtrView&&) = default;
    ComplexArrayPtrView& operator=(ComplexArrayPtrView&&) = default;

    // Almost the same as std::span<const std::complex<T>>, but don't provide
    // data() as ComplexParallelPtrView stores complex numbers in parallel arrays. 
    //const value_type* data() const noexcept = delete;

    constexpr auto real(size_type idx) const noexcept // see https://en.cppreference.com/w/cpp/numeric/complex/real2
    {
        return data_[idx].real();
    }
    constexpr auto imag(size_type idx) const noexcept // see https://en.cppreference.com/w/cpp/numeric/complex/imag2
    {
        return data_[idx].imag();
    }

    constexpr const auto& operator[](size_type idx) const noexcept
    {
        assert(idx < size());
        return data_[idx];
    }

    constexpr size_type size() const noexcept
    {
        return data_.size();
    }

private:
    span_t data_;
};

template <typename TData>
struct ComplexArrayView final
{
    using size_type = size_t;
    using value_type = typename TData::value_type;

    ComplexArrayView() = delete;
    ~ComplexArrayView() = default;
    using span_t = coda_oss::span<const value_type>;
    explicit ComplexArrayView(const TData& data) :
        view(span_t(data.data(), data.size()))
    {
    }
    ComplexArrayView(const ComplexArrayView&) = default;
    ComplexArrayView& operator=(const ComplexArrayView&) = default;
    ComplexArrayView(ComplexArrayView&&) = default;
    ComplexArrayView& operator=(ComplexArrayView&&) = default;

    constexpr auto real(size_type idx) const noexcept // see https://en.cppreference.com/w/cpp/numeric/complex/real2
    {
        return view.real(idx);
    }
    constexpr auto imag(size_type idx) const noexcept // see https://en.cppreference.com/w/cpp/numeric/complex/imag2
    {
        return view.imag(idx);
    }

    constexpr const auto& operator[](size_type idx) const noexcept
    {
        return view[idx];
    }

    constexpr size_type size() const noexcept
    {
        return view.size();
    }

    private:
    ComplexArrayPtrView<typename value_type::value_type> view;
};

template <typename T>
struct ComplexParallelPtrView final
{
    using size_type = size_t;
    using value_type = T;

    ComplexParallelPtrView() = delete;
    ~ComplexParallelPtrView() = default;
    using span_t = coda_oss::span<const value_type>;
    ComplexParallelPtrView(span_t reals, span_t imags) :
        reals_(reals), imags_(imags)
    {
        if (reals_.size() != imags_.size())
        {
            throw std::invalid_argument("real and imag must be the same size.");
        }
    }
    ComplexParallelPtrView(const T* reals, const T* imags, size_t sz) :
        ComplexParallelPtrView(span_t(reals, sz), span_t(imags, sz))
    {
    }
    ComplexParallelPtrView(const ComplexParallelPtrView&) = default;
    ComplexParallelPtrView& operator=(const ComplexParallelPtrView&) = default;
    ComplexParallelPtrView(ComplexParallelPtrView&&) = default;
    ComplexParallelPtrView& operator=(ComplexParallelPtrView&&) = default;

    constexpr auto real(size_type idx) const noexcept // see https://en.cppreference.com/w/cpp/numeric/complex/real2
    {
        assert(idx < reals_.size());
        return reals_[idx];
    }
    constexpr auto imag(size_type idx) const noexcept  // see https://en.cppreference.com/w/cpp/numeric/complex/imag2
    {
        assert(idx < imags_.size());
        return imags_[idx];
    }

    constexpr auto operator[](size_type idx) const noexcept
    {
        assert(idx < size());

        // Note that this is a COPY because the "real" and "imag" parts MUST be
        // next to each other. https://en.cppreference.com/w/cpp/numeric/complex
        return std::complex<value_type>(real(idx), imag(idx));
    }

    constexpr size_type size() const noexcept
    {
        assert(reals_.size() == imags_.size());
        return reals_.size();
    }

    private:
    span_t reals_;
    span_t imags_;
};

template <typename TData> // e.g., std::vector<float>
struct ComplexParallelView final
{
    using size_type = size_t;
    using value_type = typename TData::value_type; // i.e., float

    ComplexParallelView() = delete;
    ~ComplexParallelView() = default;
    using span_t = coda_oss::span<const value_type>;
    ComplexParallelView(const TData& reals, const TData& imags) :
        view(span_t(reals.data(), reals.size()), span_t(imags.data(), imags.size()))
    {
    }
    ComplexParallelView(const ComplexParallelView&) = default;
    ComplexParallelView& operator=(const ComplexParallelView&) = default;
    ComplexParallelView(ComplexParallelView&&) = default;
    ComplexParallelView& operator=(ComplexParallelView&&) = default;

    constexpr auto real(size_type idx) const noexcept // see https://en.cppreference.com/w/cpp/numeric/complex/real2
    {
        return view.real(idx);
    }
    constexpr auto imag(size_type idx) const noexcept // see https://en.cppreference.com/w/cpp/numeric/complex/imag2
    {
        return view.imag(idx);
    }

    constexpr auto operator[](size_type idx) const noexcept
    {
        return view[idx];
    }

    constexpr size_type size() const noexcept
    {
        return view.size();
    }

    private:
    ComplexParallelPtrView<value_type> view;
};

} 

#endif  // CODA_OSS_mem_ComplexView_h_INCLUDED_
