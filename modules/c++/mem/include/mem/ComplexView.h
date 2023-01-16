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
#include <stdint.h>

#include <vector>
#include <complex>
#include <stdexcept>
#include <functional>
#include <iterator>

#include "coda_oss/span.h"

namespace mem
{

 /*!
 *  \class ComplexViewConstIterator
 *  \brief Iterate over a ComplexView class only needing to know the
 * "axis" of the std::complex<>, i.e., "float" or "double".
 * 
 * This allows view-independent code to be written without having to use
 * a TView-template; instead ComplexViewConstIterator<T> can be used.
 * And often such code is dependent on T (i.e., "float", but not "double")
 * which means implementation can be in a CPP file.
 */
template <typename TAxis>
struct ComplexViewConstIterator final
{
    // https://stackoverflow.com/questions/8054273/how-to-implement-an-stl-style-iterator-and-avoid-common-pitfalls
    using iterator_category = std::random_access_iterator_tag;
    using value_type = std::complex<TAxis>;
    using difference_type = ptrdiff_t;
    using size_type = size_t;
    using pointer = const value_type*;
    using reference = const value_type&;

    template <typename TView>
    explicit ComplexViewConstIterator(TView view) :
        size_(view.size()) // Saving the size avoids the need for another constructor.
    {
        index_f_ = [view](size_type i) { return view[i]; };        
    }
    // Help ensure the iterators use the same view
    auto end_() const
    {
        auto retval = *this;
        retval.index_ = size_;
        return retval;
    }

    ComplexViewConstIterator() = default;
    ~ComplexViewConstIterator() = default;
    ComplexViewConstIterator(const ComplexViewConstIterator&) = default;
    ComplexViewConstIterator& operator=(const ComplexViewConstIterator&) = default;
    ComplexViewConstIterator(ComplexViewConstIterator&&) = default;
    ComplexViewConstIterator& operator=(ComplexViewConstIterator&&) = default;

    bool operator==(const ComplexViewConstIterator& rhs) const
    {
        // Checking the target() helps ensure the same type of view is used.
        return (index_ == rhs.index_) && (size_ == rhs.size_) &&
                (index_f_.target_type() == rhs.index_f_.target_type());
    }
    bool operator!=(const ComplexViewConstIterator& rhs) const
    {
        return !(*this == rhs);
    }

    ComplexViewConstIterator& operator++()
    {
        ++index_;
        return *this;
    }
    ComplexViewConstIterator operator++(int)
    {
        auto retval = *this;
        ++(*this);
        return retval;
    }
    ComplexViewConstIterator& operator+=(size_type i)
    {
        index_ += i;
        return *this;
    }

    difference_type operator-(const ComplexViewConstIterator& other) const
    {
        return index_ - other.index_;
    }

    reference operator*() const
    {
        return *(current());
    }

    pointer operator->() const
    {
        return current();
    }

private:
    // Need to store the current value because ->() returns a pointer
    mutable value_type current_value_;
    pointer current() const
    {
        current_value_ = index_f_(index_);
        return &current_value_;
    }

    size_type index_ = 0;
    std::function<value_type(size_t)> index_f_;
    size_type size_;  // used to create the end_() iterator
};

 /*!
 *  \class ComplexView
 *  \brief These classes class provide read-only views onto a collection of complex
 * numbers. For the simple case, it's roughtly std::span<const std::complex<T>>. However,
 * sometimes the data is in two parallel arrays:
 *    const float* reals;
 *    const float* imags;
 * This classes have (almost) the same access routines regardless of how the underlying data is actually stored.
 * 
 * Thing are intentinally kept very simple (for now), because it's not yet clear
 * what functionality will be needed; YAGNI.
 */

template <typename T>
struct ComplexSpanView final
{
    using size_type = size_t;
    using value_type = std::complex<T>;
    using cxvalue_t_ = value_type;
    using span_t_ = coda_oss::span<const cxvalue_t_>;
    using axis_t_ = typename cxvalue_t_::value_type; // i.e., float
    using const_iterator = ComplexViewConstIterator<axis_t_>;
    using iterator = const_iterator;

    ComplexSpanView() = delete;
    ~ComplexSpanView() = default;
    explicit ComplexSpanView(span_t_ data) : data_(data)
    {
    }
    ComplexSpanView(const ComplexSpanView&) = default;
    ComplexSpanView& operator=(const ComplexSpanView&) = default;
    ComplexSpanView(ComplexSpanView&&) = default;
    ComplexSpanView& operator=(ComplexSpanView&&) = default;

    // This class is almost the same as std::span<const std::complex<T>> ... but
    // don't provide data() as ComplexSpansView stores complex numbers in two
    // parallel arrays. Also see reals(), imags() and values(); below.
    // const value_type* data() const noexcept = delete;

    constexpr auto real(size_type idx) const noexcept
    {
        return data_[idx].real();
    }
    constexpr auto imag(size_type idx) const noexcept
    {
        return data_[idx].imag();
    }

    // const& vs. value makes little difference here as cxvalue_type is
    // std::complex<> (copying one pointer vs. two doubles).  But this
    // provides a (subtle) way for clients to know what "view style" they're
    // using, should that be needed; it also more closely matches std::span<>.
    constexpr const cxvalue_t_& index(size_type idx) const noexcept // i.e., std::complex<float>
    {
        return data_[idx];
    }
    constexpr const auto& operator[](size_type idx) const noexcept
    {
        return index(idx);
    }

    constexpr size_type size() const noexcept
    {
        return data_.size();
    }
    
    const_iterator begin() const
    {
        // not data_.begin(), we want our "generic" ComplexViewConstIterator iterator
        return ComplexViewConstIterator<axis_t_>(data_);
    }
    const_iterator end() const
    {
        return begin().end_();
    }

private:
    template <typename TAxisFunc>
    auto copy_axis(TAxisFunc axis) const
    {
        std::vector<axis_t_> retval(size());
        for (size_t i = 0; i < size(); i++)
        {
            retval[i] = (this->*axis)(i);  // call via pointer to member function
        }
        return retval;
    }

public:
    auto reals() const
    {
        return copy_axis(&ComplexSpanView::real);
    }
    auto imags() const
    {
        return copy_axis(&ComplexSpanView::imag);
    }
    auto values() const
    {
        return std::vector<cxvalue_t_>(data_.begin(), data_.end());
    }

private:
    span_t_ data_; // i.e., std::span<const std::complex<float>>
};
template <typename T>
inline auto make_ComplexSpanView(coda_oss::span<const std::complex<T>> s)
{
    return ComplexSpanView<T>(s);
}

template <typename TVectorLike>
struct ComplexArrayView final
{
    using size_type = size_t;
    using value_type = typename TVectorLike::value_type;
    using cxvalue_t_ = value_type;
    using span_t_ = coda_oss::span<const cxvalue_t_>;
    using axis_t_ = typename cxvalue_t_::value_type;  // i.e., float
    using view_t_ = ComplexSpanView<axis_t_>;
    using const_iterator = typename view_t_::const_iterator;
    using iterator = const_iterator;

    ComplexArrayView() = delete;
    ~ComplexArrayView() = default;
    explicit ComplexArrayView(const TVectorLike& data) : view_(span_t_(data.data(), data.size()))
    {
    }
    ComplexArrayView(const ComplexArrayView&) = default;
    ComplexArrayView& operator=(const ComplexArrayView&) = default;
    ComplexArrayView(ComplexArrayView&&) = default;
    ComplexArrayView& operator=(ComplexArrayView&&) = default;

    constexpr auto real(size_type idx) const noexcept
    {
        return view_.real(idx);
    }
    constexpr auto imag(size_type idx) const noexcept
    {
        return view_.imag(idx);
    }

    constexpr const cxvalue_t_& index(size_type idx) const noexcept // i.e., std::complex<float>
    {
        return view_[idx];
    }
    constexpr const auto& operator[](size_type idx) const noexcept
    {
        return index(idx);
    }

    constexpr size_type size() const noexcept
    {
        return view_.size();
    }

    auto begin() const
    {
        return view_.begin();
    }
    auto end() const
    {
        return view_.end();
    }

    auto reals() const
    {
        return view_.reals();
    }
    auto imags() const
    {
        return view_.imags();
    }
    auto values() const
    {
        return view_.values();
    }

private:
    view_t_ view_;  // i.e., ComplexSpanView<float>
};
template <typename TVectorLike>
inline auto make_ComplexArrayView(const TVectorLike& v)
{
    return ComplexArrayView<TVectorLike>(v);
}

template <typename T>
struct ComplexSpansView final // "Span_s_,", i.e., two spans. Avoiding "parallel" because that can conjure up multi-threading thoughts.
{
    using size_type = size_t;
    using value_type = T;
    using cxvalue_t_ = std::complex<value_type>;
    using axis_t_ = typename cxvalue_t_::value_type;  // i.e., float
    using span_t_ = coda_oss::span<const value_type>;
    using const_iterator = ComplexViewConstIterator<axis_t_>;
    using iterator = const_iterator;

    ComplexSpansView() = delete;
    ~ComplexSpansView() = default;
    ComplexSpansView(span_t_ reals, span_t_ imags) : reals_(reals), imags_(imags)
    {
        if (reals_.size() != imags_.size())
        {
            throw std::invalid_argument("real and imag must be the same size.");
        }
    }
    ComplexSpansView(const T* reals, const T* imags, size_t sz) :
        ComplexSpansView(span_t_(reals, sz), span_t_(imags, sz))
    {
    }
    ComplexSpansView(const ComplexSpansView&) = default;
    ComplexSpansView& operator=(const ComplexSpansView&) = default;
    ComplexSpansView(ComplexSpansView&&) = default;
    ComplexSpansView& operator=(ComplexSpansView&&) = default;

    constexpr auto real(size_type idx) const noexcept
    {
        return reals_[idx];
    }
    constexpr auto imag(size_type idx) const noexcept
    {
        return imags_[idx];
    }

    constexpr cxvalue_t_ index(size_type idx) const noexcept // i.e., std::complex<float>
    {
        // Note that this is a COPY because the "real" and "imag" parts MUST be
        // next to each other. https://en.cppreference.com/w/cpp/numeric/complex
        return cxvalue_t_(real(idx), imag(idx));
    }
    constexpr auto operator[](size_type idx) const noexcept
    {
        return index(idx);
    }

    constexpr size_type size() const noexcept
    {
        return reals_.size();
    }

    iterator begin() const
    {
        return iterator(*this);
    }
    iterator end() const
    {
        return begin().end_();
    }

    auto reals() const
    {
        return std::vector<value_type>(reals_.begin(), reals_.end());
    }
    auto imags() const
    {
        return std::vector<value_type>(imags_.begin(), imags_.end());
    }
    auto values() const
    {
        std::vector<cxvalue_t_> retval(size());
        for (size_t i = 0; i < size(); i++)
        {
            retval[i] = (*this)[i];
        }
        return retval;
    }

private:
    span_t_ reals_; // i.e., std::span<const float>
    span_t_ imags_;
};
template <typename T>
inline auto make_ComplexSpansView(coda_oss::span<const T> reals, coda_oss::span<const T> imags)
{
    return ComplexSpansView<T>(reals, imags);
}

template <typename TVectorLike>  // e.g., std::vector<float>
struct ComplexArraysView final // "Array_s_,", i.e., two arrays. Avoiding "parallel" because that can conjure up multi-threading thoughts.
{
    using size_type = size_t;
    using value_type = typename TVectorLike::value_type;  // i.e., float
    using cxvalue_t_ = std::complex<value_type>;
    using axis_t_ = typename cxvalue_t_::value_type;  // i.e., float
    using span_t_ = coda_oss::span<const value_type>;
    using view_t_ = ComplexSpansView<axis_t_>;
    using const_iterator = typename view_t_::const_iterator;
    using iterator = const_iterator;

    ComplexArraysView() = delete;
    ~ComplexArraysView() = default;
    ComplexArraysView(const TVectorLike& reals, const TVectorLike& imags) :
        view(span_t_(reals.data(), reals.size()), span_t_(imags.data(), imags.size()))
    {
    }
    ComplexArraysView(const ComplexArraysView&) = default;
    ComplexArraysView& operator=(const ComplexArraysView&) = default;
    ComplexArraysView(ComplexArraysView&&) = default;
    ComplexArraysView& operator=(ComplexArraysView&&) = default;

    constexpr auto real(size_type idx) const noexcept
    {
        return view.real(idx);
    }
    constexpr auto imag(size_type idx) const noexcept
    {
        return view.imag(idx);
    }

    constexpr cxvalue_t_ index(size_type idx) const noexcept // i.e., std::complex<float>
    {
        return view[idx];
    }
    constexpr auto operator[](size_type idx) const noexcept
    {
        return index(idx);
    }

    constexpr size_type size() const noexcept
    {
        return view.size();
    }

    auto begin() const
    {
        return view.begin();
    }
    auto end() const
    {
        return view.end();
    }

    auto reals() const
    {
        return view.reals();
    }
    auto imags() const
    {
        return view.imags();
    }
    auto values() const
    {
        return view.values();
    }

private:
    view_t_ view;  // i.e., ComplexSpansView<float>
};
template <typename TVectorLike>
inline auto make_ComplexArraysView(const TVectorLike& reals, const TVectorLike& imags)
{
    return ComplexArraysView<TVectorLike>(reals, imags);
}

} 

#endif  // CODA_OSS_mem_ComplexView_h_INCLUDED_
