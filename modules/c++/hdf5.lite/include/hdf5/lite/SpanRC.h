/* =========================================================================
 * This file is part of hdf5.lite-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2022, Maxar Technologies, Inc.
 *
 * hdf5.lite-c++ is free software; you can redistribute it and/or modify
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
#ifndef CODA_OSS_hdf5_lite_SpanRC_h_INCLUDED_
#define CODA_OSS_hdf5_lite_SpanRC_h_INCLUDED_

/*!
 * \file  SpanRC.h
 *
 * This is a super-simple version of C++23's mdspan.  It's here because 1) don't want widespread use, and
 * 2) CODA already has a View2D.
 */

#include <assert.h>

#include <stdexcept>
#include <array>
#include <utility>

#include "config/Exports.h"
#include "coda_oss/span.h"
#include "types/RowCol.h"
#include "coda_oss/mdspan.h"

namespace hdf5
{
namespace lite
{
namespace details
{
//#if CODA_OSS_cpp20 && CODA_OSS_mdspan
// https://github.com/kokkos/mdspan/wiki/A-Gentle-Introduction-to-mdspan
template <typename T>
using msdpan_2 = coda_oss::mdspan<T, coda_oss::dextents<size_t, 2>>;
//#endif

template <typename T>
struct SpanRC final // TODO: use std::mdspan
{
    using size_type = size_t;
    using element_type = T;
    using pointer = T*;
    using reference = T&;

    SpanRC() = default;
    SpanRC(pointer p, const std::array<size_type, 2>& dims) noexcept : SpanRC(p, types::RowCol<size_type>(dims))
    {
    }
    SpanRC(const SpanRC&) noexcept = default;

    constexpr pointer data_handle() const noexcept
    {
        return s_.data();
    }

    /*constexpr*/ reference operator[](size_t idx) const noexcept
    {
        assert(idx < size());  // prevents "constexpr" in C++11
        return data_handle()[idx];
    }
    /*constexpr*/ reference operator()(size_t r, size_t c) const noexcept
    {
        const auto offset = (r * rc_.col) + c;
        return (*this)[offset];
    }

    constexpr size_t size() const noexcept
    {
        assert(s_.size() == rc_.area());
        return s_.size();
    }

    constexpr bool empty() const noexcept
    {
        return s_.empty();
    }

    static constexpr auto rank() noexcept
    {
        return 2;
    }

    auto extent(size_t rank) const
    {
        if (rank == 0) return rc_.row;
        if (rank == 1) return rc_.col;
        throw std::invalid_argument("rank");
    }

private:
    coda_oss::span<T> s_;
    types::RowCol<size_type> rc_;
    SpanRC(pointer p, types::RowCol<size_type> rc) noexcept : s_(p, rc.area()), rc_(std::move(rc))
    {
    }
};
}

template<typename T>
using SpanRC = details::msdpan_2<T>;

}
}

#endif // CODA_OSS_hdf5_lite_SpanRC_h_INCLUDED_
