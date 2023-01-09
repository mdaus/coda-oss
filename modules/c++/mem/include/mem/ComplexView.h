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
 *  \brief This class provides a read-only view onto a collection of complex
 * numbers. For the simple case, it's std::span<const std::complex<T>>. However,
 * sometimes the data is in two parallel arrays: const float* reals; const
 * float* imags; This class sets things up so that clients can always use the
 * same access routines, regardless of how the underlying data is actually
 * stored.
 */
namespace details
{
template <typename TData>
struct ComplexArrayView
{
    using size_type = size_t;
    using value_type = typename TData::value_type;

    ComplexArrayView() = delete;
    ~ComplexArrayView() = default;
    explicit ComplexArrayView(const TData& data) : data_(data)
    {
    }
    ComplexArrayView(const ComplexArrayView&) = delete;
    ComplexArrayView& operator=(const ComplexArrayView&) = delete;
    ComplexArrayView(ComplexArrayView&&) = default;
    ComplexArrayView& operator=(ComplexArrayView&&) = default;

    constexpr const auto& operator[](size_type idx) const noexcept
    {
        assert(idx < size());
        return data_[idx];
    }

    constexpr size_type size() const noexcept
    {
        return data_.size();
    }

    const TData& data_;
};

template <typename TData>
struct ComplexParallelView
{
    using size_type = size_t;
    using value_type = typename TData::value_type;

    ComplexParallelView() = delete;
    ~ComplexParallelView() = default;
    explicit ComplexParallelView(const TData& reals, const TData& imags) :
        reals_(reals), imags_(imags)
    {
        if (reals_.size() != imags_.size())
        {
            throw std::invalid_argument("real and imag must be the same size.");
        }
    }
    ComplexParallelView(const ComplexParallelView&) = delete;
    ComplexParallelView& operator=(const ComplexParallelView&) = delete;
    ComplexParallelView(ComplexParallelView&&) = default;
    ComplexParallelView& operator=(ComplexParallelView&&) = default;

    constexpr auto operator[](size_type idx) const noexcept
    {
        assert(idx < size());
        assert(size() == reals_.size());
        assert(size() == imags_.size());

        // Note that this is a COPY because the "real" and "imag" parts MUST be next to each other.
        // https://en.cppreference.com/w/cpp/numeric/complex
        return std::complex<value_type>(reals_[idx], imags_[idx]);
    }

    constexpr size_type size() const noexcept
    {
        return reals_.size();
    }

    const TData& reals_;
    const TData& imags_;
};

// see https://en.cppreference.com/w/cpp/numeric/complex/real2
template <typename TView>
constexpr auto real(const TView& v, size_t idx)
{
    return v[idx].real();
}
// see https://en.cppreference.com/w/cpp/numeric/complex/imag2
template <typename TView>
constexpr auto imag(const TView& v, size_t idx)
{
    return v[idx].imag();
}

} // namespace details

template <typename T>
struct ComplexView
{
    // c.f. std::span
    using size_type = size_t;
    using element_type = std::complex<T>;
    using pointer = const element_type*;
    using reference = const element_type&;

    ComplexView() = default;
    ~ComplexView() = default;
    ComplexView(const ComplexView&) = default;
    ComplexView& operator=(const ComplexView&) = default;
    ComplexView(ComplexView&&) = default;
    ComplexView& operator=(ComplexView&&) = default;

    explicit ComplexView(coda_oss::span<const std::complex<T>> s_cx) :
        sz_(s_cx.size())
    {
    }
    explicit ComplexView(coda_oss::span<std::complex<T>> s_cx) :
        sz_(s_cx.size())
    {
    }
    explicit ComplexView(const std::vector<std::complex<T>>& v_cx) :
        sz_(v_cx.size())
    {
    }
    
    ComplexView(coda_oss::span<const T> real, coda_oss::span<const T> imag) :
        sz_(real.size())
    {
        if (imag.size() != size())
        {
            throw std::invalid_argument("real and imag must be the same size.");
        }
    }
    ComplexView(coda_oss::span<T> real, coda_oss::span<T> imag) :
        sz_(real.size())
    {
        if (imag.size() != size())
        {
            throw std::invalid_argument("real and imag must be the same size.");
        }
    }
    ComplexView(const std::vector<T>& v_real, const std::vector<T>& v_imag) :
        sz_(v_real.size())
    {
        if (v_imag.size() != size())
        {
            throw std::invalid_argument("v_real and v_imag must be the same size.");        
        }
    }

    pointer data() const noexcept = delete;  // data might be in two parallel arrays

    // https://en.cppreference.com/w/cpp/container/span/operator_at
    constexpr reference operator[](size_type idx) const noexcept
    {
        assert(idx < size());
        throw std::exception("not implmeneted");
    }

    // https://en.cppreference.com/w/cpp/container/span/size
    constexpr size_type size() const noexcept
    {
        return sz_;
    }

    // https://en.cppreference.com/w/cpp/container/span/size_bytes
    constexpr size_type size_bytes() const noexcept
    {
        return sz_ * sizeof(element_type);
    }

    // https://en.cppreference.com/w/cpp/container/span/empty
    constexpr bool empty() const noexcept
    {
        return size() == 0;
    }

private:
    size_type sz_ = 0;
};

} 

#endif  // CODA_OSS_mem_ComplexView_h_INCLUDED_
