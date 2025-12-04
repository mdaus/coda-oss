/* =========================================================================
 * This file is part of mem-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

#include <vector>
#include <complex>
#include <span>
#include <type_traits>

#include <mem/VectorOfPointers.h>
#include <mem/ComplexView.h>

#include <catch2/catch_test_macros.hpp>

#define TEST_ASSERT_EQ(X, Y) CHECK(X == Y);
#define TEST_ASSERT_NOT_EQ(X, Y) CHECK(X != Y);
#define TEST_ASSERT_NULL(X) CHECK(X == nullptr);
#define TEST_ASSERT_NOT_NULL(X) CHECK(X != nullptr);

TEST_CASE("testVecOfRawPointers")
{
    mem::VectorOfPointers<int> myVec;

    myVec.push_back(new int(1));
    myVec.push_back(new int(2));
    myVec.push_back(new int(3));
    myVec.push_back(new int(4));
    myVec.push_back(new int(5));

    TEST_ASSERT_EQ(myVec.size(), static_cast<size_t>(5));

    myVec.erase(myVec.begin() + 2);

    TEST_ASSERT_EQ(myVec.size(), static_cast<size_t>(4));
    TEST_ASSERT_EQ(*myVec[2], 4);

    myVec.push_back(new int(6));

    TEST_ASSERT_EQ(myVec.size(), static_cast<size_t>(5));

    myVec.erase(myVec.begin(), myVec.begin() + 4);

    TEST_ASSERT_EQ(myVec.size(), static_cast<size_t>(1));
    TEST_ASSERT_EQ(*myVec[0], 6);

    myVec.erase(myVec.begin());
    CHECK(myVec.empty());
}

static mem::VectorOfSharedPointers<int> mem_VectorOfSharedPointers_int()
{
    mem::VectorOfSharedPointers<int> retval;
    retval.push_back(new int(1));
    return retval;
}

static std::vector<std::shared_ptr<int>> std_vector_shared_ptr_int()
{
    std::vector<std::shared_ptr<int>> retval;
    retval.push_back(std::make_shared<int>(1));
    return retval;
}

TEST_CASE("testVecOfSharedPointers")
{
    mem::VectorOfSharedPointers<int> myVec;

    myVec.push_back(new int(1));
    myVec.push_back(new int(2));
    myVec.push_back(new int(3));
    myVec.push_back(new int(4));
    myVec.push_back(new int(5));

    TEST_ASSERT_EQ(myVec.size(), static_cast<size_t>(5));

    myVec.erase(myVec.begin() + 2);

    TEST_ASSERT_EQ(myVec.size(), static_cast<size_t>(4));
    TEST_ASSERT_EQ(*myVec[2], 4);

    myVec.push_back(new int(6));

    TEST_ASSERT_EQ(myVec.size(), static_cast<size_t>(5));

    myVec.erase(myVec.begin(), myVec.begin() + 4);

    TEST_ASSERT_EQ(myVec.size(), static_cast<size_t>(1));
    TEST_ASSERT_EQ(*myVec[0], 6);

    myVec.erase(myVec.begin());
    CHECK(myVec.empty());

    {
        mem::VectorOfSharedPointers<int> myVec2 = mem_VectorOfSharedPointers_int();  // copy
        myVec = mem_VectorOfSharedPointers_int();  // assignment
    }
    {
        mem::VectorOfSharedPointers<int> myVec2 = std_vector_shared_ptr_int();  // copy
        myVec = std_vector_shared_ptr_int();  // assignment
    }

    {
       std::vector<std::shared_ptr<int>> myVec2 = mem_VectorOfSharedPointers_int();  // copy
    }
    {
       std::vector<std::shared_ptr<int>> myVec2 = std_vector_shared_ptr_int();  // copy
    }
}

template<typename TView>
static void test_cx_view(const std::string& testName, const TView& view)
{
    TEST_ASSERT_EQ(4, view.size());
    TEST_ASSERT_EQ(view[0].real(), 1.0f);
    TEST_ASSERT_EQ(view[0].imag(), 2.0f);
    TEST_ASSERT_EQ(view[1].real(), 3.0f);
    TEST_ASSERT_EQ(view[1].imag(), 4.0f);
    TEST_ASSERT_EQ(view[2].real(), 5.0f);
    TEST_ASSERT_EQ(view[2].imag(), 6.0f);
    TEST_ASSERT_EQ(view[3].real(), 7.0f);
    TEST_ASSERT_EQ(view[3].imag(), 8.0f);
}
template <typename TView>
static void test_mem_ComplexParallelView(const std::string& testName, const TView& view)
{
    TEST_ASSERT_EQ(4, view.size());
    TEST_ASSERT_EQ(view[0].real(), 1.0f);
    TEST_ASSERT_EQ(view[0].imag(), 2.0f);
    TEST_ASSERT_EQ(view[1].real(), 3.0f);
    TEST_ASSERT_EQ(view[1].imag(), 4.0f);
    TEST_ASSERT_EQ(view[2].real(), 5.0f);
    TEST_ASSERT_EQ(view[2].imag(), 6.0f);
    TEST_ASSERT_EQ(view[3].real(), 7.0f);
    TEST_ASSERT_EQ(view[3].imag(), 8.0f);

    TEST_ASSERT_EQ(view.real(0), 1.0f);
    TEST_ASSERT_EQ(view.imag(0), 2.0f);
    TEST_ASSERT_EQ(view.real(1), 3.0f);
    TEST_ASSERT_EQ(view.imag(1), 4.0f);
    TEST_ASSERT_EQ(view.real(2), 5.0f);
    TEST_ASSERT_EQ(view.imag(2), 6.0f);
    TEST_ASSERT_EQ(view.real(3), 7.0f);
    TEST_ASSERT_EQ(view.imag(3), 8.0f);

    const auto reals_ = view.reals();
    const auto imags_ = view.imags();
    TEST_ASSERT_EQ(reals_.size(), imags_.size());
    TEST_ASSERT_EQ(reals_[0], 1.0f);
    TEST_ASSERT_EQ(reals_[1], 3.0f);
    TEST_ASSERT_EQ(reals_[2], 5.0f);
    TEST_ASSERT_EQ(reals_[3], 7.0f);

    TEST_ASSERT_EQ(imags_[0], 2.0f);
    TEST_ASSERT_EQ(imags_[1], 4.0f);
    TEST_ASSERT_EQ(imags_[2], 6.0f);
    TEST_ASSERT_EQ(imags_[3], 8.0f);
}

using cx_float = std::complex<float>;
static const std::vector<cx_float>& cx_data()
{
    static const std::vector<cx_float> retval {{1, 2}, {3, 4}, {5, 6}, {7, 8}};
    return retval;
}

TEST_CASE("testSpanCxFloat")
{
    const std::span<const cx_float> view(cx_data().data(), cx_data().size());
    TEST_ASSERT_EQ(cx_data().size(), view.size());
    TEST_ASSERT_EQ(4, view.size());
    TEST_ASSERT_EQ(view[0].real(), 1.0f);
    TEST_ASSERT_EQ(view[0].imag(), 2.0f);
    TEST_ASSERT_EQ(view[1].real(), 3.0f);
    TEST_ASSERT_EQ(view[1].imag(), 4.0f);
    TEST_ASSERT_EQ(view[2].real(), 5.0f);
    TEST_ASSERT_EQ(view[2].imag(), 6.0f);
    TEST_ASSERT_EQ(view[3].real(), 7.0f);
    TEST_ASSERT_EQ(view[3].imag(), 8.0f);
}

TEST_CASE("testComplexParallelViewFloat")
{
    {
        const auto view = mem::make_ComplexInterleavedView(cx_data());
        TEST_ASSERT_EQ(cx_data().size(), view.size());


        TEST_ASSERT_EQ(4, view.size());
        TEST_ASSERT_EQ(view[0].real(), 1.0f);
        TEST_ASSERT_EQ(view[0].imag(), 2.0f);
        TEST_ASSERT_EQ(view[1].real(), 3.0f);
        TEST_ASSERT_EQ(view[1].imag(), 4.0f);
        TEST_ASSERT_EQ(view[2].real(), 5.0f);
        TEST_ASSERT_EQ(view[2].imag(), 6.0f);
        TEST_ASSERT_EQ(view[3].real(), 7.0f);
        TEST_ASSERT_EQ(view[3].imag(), 8.0f);

        TEST_ASSERT_EQ(view.real(0), 1.0f);
        TEST_ASSERT_EQ(view.imag(0), 2.0f);
        TEST_ASSERT_EQ(view.real(1), 3.0f);
        TEST_ASSERT_EQ(view.imag(1), 4.0f);
        TEST_ASSERT_EQ(view.real(2), 5.0f);
        TEST_ASSERT_EQ(view.imag(2), 6.0f);
        TEST_ASSERT_EQ(view.real(3), 7.0f);
        TEST_ASSERT_EQ(view.imag(3), 8.0f);

        const auto reals_ = view.reals();
        const auto imags_ = view.imags();
        TEST_ASSERT_EQ(reals_.size(), imags_.size());
        TEST_ASSERT_EQ(reals_[0], 1.0f);
        TEST_ASSERT_EQ(reals_[1], 3.0f);
        TEST_ASSERT_EQ(reals_[2], 5.0f);
        TEST_ASSERT_EQ(reals_[3], 7.0f);

        TEST_ASSERT_EQ(imags_[0], 2.0f);
        TEST_ASSERT_EQ(imags_[1], 4.0f);
        TEST_ASSERT_EQ(imags_[2], 6.0f);
        TEST_ASSERT_EQ(imags_[3], 8.0f);


        TEST_ASSERT_EQ(4, view.size());
        TEST_ASSERT_EQ(view[0].real(), 1.0f);
        TEST_ASSERT_EQ(view[0].imag(), 2.0f);
        TEST_ASSERT_EQ(view[1].real(), 3.0f);
        TEST_ASSERT_EQ(view[1].imag(), 4.0f);
        TEST_ASSERT_EQ(view[2].real(), 5.0f);
        TEST_ASSERT_EQ(view[2].imag(), 6.0f);
        TEST_ASSERT_EQ(view[3].real(), 7.0f);
        TEST_ASSERT_EQ(view[3].imag(), 8.0f);
    }

    const std::vector<float> reals{1, 3, 5, 7};
    const std::vector<float> imags{2, 4, 6, 8};
    TEST_ASSERT_EQ(imags.size(), reals.size());
    {
        const auto view = mem::make_ComplexParallelView(reals, imags);
        TEST_ASSERT_EQ(reals.size(), view.size());
        

        TEST_ASSERT_EQ(4, view.size());
        TEST_ASSERT_EQ(view[0].real(), 1.0f);
        TEST_ASSERT_EQ(view[0].imag(), 2.0f);
        TEST_ASSERT_EQ(view[1].real(), 3.0f);
        TEST_ASSERT_EQ(view[1].imag(), 4.0f);
        TEST_ASSERT_EQ(view[2].real(), 5.0f);
        TEST_ASSERT_EQ(view[2].imag(), 6.0f);
        TEST_ASSERT_EQ(view[3].real(), 7.0f);
        TEST_ASSERT_EQ(view[3].imag(), 8.0f);

        TEST_ASSERT_EQ(view.real(0), 1.0f);
        TEST_ASSERT_EQ(view.imag(0), 2.0f);
        TEST_ASSERT_EQ(view.real(1), 3.0f);
        TEST_ASSERT_EQ(view.imag(1), 4.0f);
        TEST_ASSERT_EQ(view.real(2), 5.0f);
        TEST_ASSERT_EQ(view.imag(2), 6.0f);
        TEST_ASSERT_EQ(view.real(3), 7.0f);
        TEST_ASSERT_EQ(view.imag(3), 8.0f);

        const auto reals_ = view.reals();
        const auto imags_ = view.imags();
        TEST_ASSERT_EQ(reals_.size(), imags_.size());
        TEST_ASSERT_EQ(reals_[0], 1.0f);
        TEST_ASSERT_EQ(reals_[1], 3.0f);
        TEST_ASSERT_EQ(reals_[2], 5.0f);
        TEST_ASSERT_EQ(reals_[3], 7.0f);

        TEST_ASSERT_EQ(imags_[0], 2.0f);
        TEST_ASSERT_EQ(imags_[1], 4.0f);
        TEST_ASSERT_EQ(imags_[2], 6.0f);
        TEST_ASSERT_EQ(imags_[3], 8.0f);


        TEST_ASSERT_EQ(4, view.size());
        TEST_ASSERT_EQ(view[0].real(), 1.0f);
        TEST_ASSERT_EQ(view[0].imag(), 2.0f);
        TEST_ASSERT_EQ(view[1].real(), 3.0f);
        TEST_ASSERT_EQ(view[1].imag(), 4.0f);
        TEST_ASSERT_EQ(view[2].real(), 5.0f);
        TEST_ASSERT_EQ(view[2].imag(), 6.0f);
        TEST_ASSERT_EQ(view[3].real(), 7.0f);
        TEST_ASSERT_EQ(view[3].imag(), 8.0f);
    }
}

TEST_CASE("testComplexParallelViewFloatIterator")
{
    {
        const auto view = mem::make_ComplexInterleavedView(cx_data());
        {
            {
                auto begin = view.begin();
                auto end = view.end();
                CHECK(begin != end);
            
                const auto distance = std::distance(begin, end);
                TEST_ASSERT_EQ(4, distance);
            
                auto it = begin;
                TEST_ASSERT_EQ((*it).real(), 1.0f);
                TEST_ASSERT_EQ((*it).imag(), 2.0f);
            
                ++it;
                TEST_ASSERT_EQ((*it).real(), 3.0f);
                TEST_ASSERT_EQ((*it).imag(), 4.0f);
            
                it++;
                TEST_ASSERT_EQ((*it).real(), 5.0f);
                TEST_ASSERT_EQ((*it).imag(), 6.0f);
            
                it += 1;
                TEST_ASSERT_EQ(it->real(), 7.0f);
                TEST_ASSERT_EQ(it->imag(), 8.0f);
            }

            using cxvalue_t = typename decltype(view.begin())::value_type; // i.e., std::complex<float>
            cxvalue_t cx{1.0f, 2.0f};
            for (auto&& v : view)
            {
                TEST_ASSERT_EQ(v.real(), cx.real());
                TEST_ASSERT_EQ(v.imag(), cx.imag());

                cx = cxvalue_t{cx.real() + 2.0f, cx.imag() + 2.0f};
            }
        }
    }

    const std::vector<float> reals{1, 3, 5, 7};
    const std::vector<float> imags{2, 4, 6, 8};
    TEST_ASSERT_EQ(imags.size(), reals.size());
    {
        const auto view = mem::make_ComplexParallelView(reals, imags);
        {
            {
                auto begin = view.begin();
                auto end = view.end();
                CHECK(begin != end);
            
                const auto distance = std::distance(begin, end);
                TEST_ASSERT_EQ(4, distance);
            
                auto it = begin;
                TEST_ASSERT_EQ((*it).real(), 1.0f);
                TEST_ASSERT_EQ((*it).imag(), 2.0f);
            
                ++it;
                TEST_ASSERT_EQ((*it).real(), 3.0f);
                TEST_ASSERT_EQ((*it).imag(), 4.0f);
            
                it++;
                TEST_ASSERT_EQ((*it).real(), 5.0f);
                TEST_ASSERT_EQ((*it).imag(), 6.0f);
            
                it += 1;
                TEST_ASSERT_EQ(it->real(), 7.0f);
                TEST_ASSERT_EQ(it->imag(), 8.0f);
            }

            using cxvalue_t = typename decltype(view.begin())::value_type; // i.e., std::complex<float>
            cxvalue_t cx{1.0f, 2.0f};
            for (auto&& v : view)
            {
                TEST_ASSERT_EQ(v.real(), cx.real());
                TEST_ASSERT_EQ(v.imag(), cx.imag());

                cx = cxvalue_t{cx.real() + 2.0f, cx.imag() + 2.0f};
            }
        }
    }
}
