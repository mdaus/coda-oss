/* =========================================================================
 * This file is part of sys-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2017, MDA Information Systems LLC
 *
 * sys-c++ is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.

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

#include <catch2/catch_test_macros.hpp>

#include <array>
#include <vector>
#include <bit> // std::endian
#include <cstddef>
#include <span>
#include <type_traits>

#include <sys/Conf.h>
#include <sys/Span.h>

TEST_CASE("testEndianness")
{
    /*const*/ auto native = std::endian::native; // "const" causes "conditional expression is constant."

    if (native == std::endian::big) { }
    else if (native == std::endian::little) { }
    else
    {
        FAIL("Mixed-endian not supported!");
    }

    const bool isBigEndianSystem = sys::isBigEndianSystem();

    if (native == std::endian::big)
    {
        CHECK(isBigEndianSystem);
    }
    else
    {
        CHECK(!isBigEndianSystem);    
    }
    if (native == std::endian::little)
    {
        CHECK(!isBigEndianSystem);
    }
    else
    {
        CHECK(isBigEndianSystem);
    }


    if (isBigEndianSystem)
    {
        CHECK(native == std::endian::big);
    }
    else
    {
        CHECK(native == std::endian::little);    
    }
}

template<typename T>
static std::vector<T> make_origValues(size_t NUM_PIXELS)
{
    ::srand(334);

    std::vector<T> retval(NUM_PIXELS);
    for (size_t ii = 0; ii < NUM_PIXELS; ++ii)
    {
        const auto value = static_cast<float>(::rand()) / RAND_MAX *
                std::numeric_limits<uint64_t>::max();
        retval[ii] = static_cast<T>(value);
    }
    return retval;
}

TEST_CASE("testByteSwapV")
{
    constexpr size_t NUM_PIXELS = 10000;
    const auto origValues = make_origValues <uint64_t>(NUM_PIXELS);

    // Byte swap the old-fashioned way
    auto values1(origValues);
    sys::byteSwap(values1.data(), sizeof(uint64_t), NUM_PIXELS);

    // Byte swap into output buffer
    std::vector<uint64_t> swappedValues2(origValues.size());
    sys::byteSwap(origValues.data(), sizeof(uint64_t), NUM_PIXELS, swappedValues2.data());

    // Everything should match
    for (size_t ii = 0; ii < NUM_PIXELS; ++ii)
    {
        CHECK(values1[ii] == swappedValues2[ii]);
    }
}

TEST_CASE("testByteSwapCxV")
{
    constexpr size_t NUM_PIXELS = 10000;
    using value_type = std::complex<float>;
    const auto origValues = make_origValues<value_type>(NUM_PIXELS);

    constexpr auto elemSize = sizeof(value_type) / 2;
    constexpr auto numElems = NUM_PIXELS * 2;

    // Byte swap the old-fashioned way
    auto values1(origValues);
    sys::byteSwap(values1.data(), elemSize, numElems);

    // Byte swap into output buffer
    std::vector<value_type> swappedValues2(origValues.size());
    sys::byteSwap(origValues.data(), elemSize, numElems, swappedValues2.data());

    // Everything should match
    for (size_t ii = 0; ii < NUM_PIXELS; ++ii)
    {
        using int_type = uint64_t;
        static_assert(sizeof(int_type) == sizeof(value_type), "Unknown sizeof(std::complex<float>)");

        // If these values are byte-swapped, they could be bogus.
        const void* const pValue1_ = &(values1[ii]);
        const void* const pSwappedValue2_ = &(swappedValues2[ii]);
        
        auto const pValue1 = static_cast<const int_type*>(pValue1_);
        auto const pSwappedValue2 = static_cast<const int_type*>(pSwappedValue2_);

        CHECK(*pValue1 == *pSwappedValue2);
    }
}

template<typename T>
inline std::span<const T> as_span(const std::vector<std::byte>& bytes)
{
    const void* const pBytes_ = bytes.data();
    auto const p = static_cast<const T*>(pBytes_);
    const auto sz = bytes.size() / sizeof(T);
    return sys::make_span(p, sz);
}

TEST_CASE("testByteSwap")
{
    constexpr size_t NUM_PIXELS = 10000;
    const auto origValues = make_origValues<uint64_t>(NUM_PIXELS);
    const auto origValues_ = sys::make_span(origValues);

    auto values1(origValues);
    sys::byteSwap(sys::make_span(values1));

    // Byte swap into output buffer
    std::vector<uint64_t> swappedValues2(origValues.size());
    sys::byteSwap(origValues_, sys::as_writable_bytes(swappedValues2));

    // std::vector<std::byte> returned
    const auto swappedValues3_ = sys::byteSwap(origValues_);
    const auto swappedValues3 = as_span<uint64_t>(swappedValues3_);

    // Everything should match
    for (size_t ii = 0; ii < NUM_PIXELS; ++ii)
    {
        CHECK(values1[ii] == swappedValues2[ii]);
        CHECK(values1[ii] == swappedValues3[ii]);
    }
}

// 0xnn is an `int` which can't be used to initialize std::byte w/o a cast
#define CODA_OSS_define_byte(v) constexpr static std::byte v = static_cast<std::byte>(0 ## v)
CODA_OSS_define_byte(x00);
CODA_OSS_define_byte(x11);
CODA_OSS_define_byte(x22);
CODA_OSS_define_byte(x33);
CODA_OSS_define_byte(x44);
CODA_OSS_define_byte(x55);
CODA_OSS_define_byte(x66);
CODA_OSS_define_byte(x77);
CODA_OSS_define_byte(x88);
CODA_OSS_define_byte(x99);
CODA_OSS_define_byte(xAA);
CODA_OSS_define_byte(xBB);
CODA_OSS_define_byte(xCC);
CODA_OSS_define_byte(xDD);
CODA_OSS_define_byte(xEE);
CODA_OSS_define_byte(xFF);
#undef CODA_OSS_define_byte

// static constexpr std::byte sixteen_bytes[]{x00, x11, x22, x33, x44, x55, x66, x77, x88, x99, xAA, xBB, xCC, xDD, xEE, xFF};

TEST_CASE("testByteSwapValuesTwoBytes")
{
    constexpr std::byte two_bytes[]{x00, xFF};
    auto pUInt = reinterpret_cast<const uint16_t*>(two_bytes);
    auto swap = sys::byteSwap(*pUInt);
    CHECK(*pUInt != swap);

    const void* pResult_ = &swap;
    auto const pResultBytes = static_cast<const std::byte*>(pResult_);
    auto const pValueBytes = static_cast<const std::byte*>(two_bytes);
    for (size_t i = 0, j = sizeof(uint16_t); i < sizeof(uint16_t) && j > 0; i++, j--)
    {
        CHECK(pResultBytes[i] == pValueBytes[j - 1]);
    }

    swap = sys::byteSwap(swap);  // swap back
    CHECK(*pUInt == swap);

    // swap as an "array" of one value
    sys::byteSwap(pUInt, sizeof(uint16_t), 1, &swap);
    CHECK(*pUInt != swap);
    sys::byteSwap(&swap, sizeof(uint16_t), 1); // swap back
    CHECK(*pUInt == swap);

    const auto resultBytes = sys::byteSwapValue(*pUInt);
    CHECK(resultBytes.size() == sizeof(uint16_t));
    for (size_t i = 0, j = sizeof(uint16_t); i < sizeof(uint16_t) && j > 0; i++, j--)
    {
        CHECK(resultBytes[i] == pValueBytes[j - 1]);
    }
}
TEST_CASE("testByteSwapValuesFourBytes")
{
    constexpr std::byte four_bytes[]{x00, x11, xEE, xFF};
    auto pUInt = reinterpret_cast<const uint32_t*>(four_bytes);
    auto swap = sys::byteSwap(*pUInt);
    CHECK(*pUInt != swap);

    const void* pResult_ = &swap;
    auto const pResultBytes = static_cast<const std::byte*>(pResult_);
    auto const pValueBytes = static_cast<const std::byte*>(four_bytes);
    for (size_t i = 0, j = sizeof(uint32_t); i < sizeof(uint32_t) && j > 0; i++, j--)
    {
        CHECK(pResultBytes[i] == pValueBytes[j - 1]);
    }

    swap = sys::byteSwap(swap);  // swap back
    CHECK(*pUInt == swap);

    // swap as an "array" of one value
    sys::byteSwap(pUInt, sizeof(uint32_t), 1, &swap);
    CHECK(*pUInt != swap);
    sys::byteSwap(&swap, sizeof(uint32_t), 1); // swap back
    CHECK(*pUInt == swap);

    const auto resultBytes = sys::byteSwapValue(*pUInt);
    CHECK(resultBytes.size() == sizeof(uint32_t));
    for (size_t i = 0, j = sizeof(uint32_t); i < sizeof(uint32_t) && j > 0; i++, j--)
    {
        CHECK(resultBytes[i] == pValueBytes[j - 1]);
    }
}
TEST_CASE("testByteSwapValuesEightBytes")
{
    constexpr std::byte eight_bytes[]{x00, x11, x22, x33, xCC, xDD, xEE, xFF};
    auto pUInt = reinterpret_cast<const uint64_t*>(eight_bytes);
    auto swap = sys::byteSwap(*pUInt);
    CHECK(*pUInt != swap);

    const void* pResult_ = &swap;
    auto const pResultBytes = static_cast<const std::byte*>(pResult_);
    auto const pValueBytes = static_cast<const std::byte*>(eight_bytes);
    for (size_t i = 0, j = sizeof(uint64_t); i < sizeof(uint64_t) && j > 0; i++, j--)
    {
        CHECK(pResultBytes[i] == pValueBytes[j - 1]);
    }

    swap = sys::byteSwap(swap);  // swap back
    CHECK(*pUInt == swap);

    // swap as an "array" of one value
    sys::byteSwap(pUInt, sizeof(uint64_t), 1, &swap);
    CHECK(*pUInt != swap);
    sys::byteSwap(&swap, sizeof(uint64_t), 1); // swap back
    CHECK(*pUInt == swap);

    const auto resultBytes = sys::byteSwapValue(*pUInt);
    CHECK(resultBytes.size() == sizeof(uint64_t));
    for (size_t i = 0, j = sizeof(uint64_t); i < sizeof(uint64_t) && j > 0; i++, j--)
    {
        CHECK(resultBytes[i] == pValueBytes[j - 1]);
    }
}

TEST_CASE("testByteSwapCxValue")
{
    using value_type = std::complex<float>;
    const value_type cx{3.14f, -31.4f}; // using raw bytes can lean to `nan`s
    auto const pValue = &cx;

    auto swap = sys::byteSwap(*pValue);
    CHECK(*pValue != swap); // technically a bit goofy as the bits may not represent `T`s
            
    swap = sys::byteSwap(swap);  // swap back
    CHECK(*pValue == swap);
}

TEST_CASE("testByteSwap12")
{
    // test a goofy element size
    constexpr std::byte twelve_bytes[]{
        x00, x11, x22, x33, x44, x55,
        x99, xAA, xBB, xDD, xEE, xFF};
    const auto pValueBytes = sys::as_bytes(twelve_bytes);
    constexpr auto extent_twelve_bytes = std::extent<decltype(twelve_bytes)>::value;

    std::vector<std::byte> swappedValues(extent_twelve_bytes);
    auto pResultBytes = sys::make_span(swappedValues);

    size_t elemSize = 12;
    auto numElements = swappedValues.size() / elemSize;
    sys::byteSwap(twelve_bytes, elemSize, numElements, pResultBytes.data());
    for (size_t i = 0, j = elemSize; i < elemSize && j > 0; i++, j--)
    {
        CHECK(pResultBytes[i] == pValueBytes[j - 1]);
    }

    // swap as a SINGLE 12-byte value
    const auto result = sys::details::swapBytes<extent_twelve_bytes>(pValueBytes, pResultBytes);
    for (size_t i = 0, j = elemSize; i < elemSize && j > 0; i++, j--)
    {
        CHECK(result[i] == pValueBytes[j - 1]);
    }

    elemSize = 6; // note that an ODD size doesn't work correctly
    numElements = swappedValues.size() / elemSize;
    sys::byteSwap(twelve_bytes, elemSize, numElements, swappedValues.data());
    {
        const auto& lRes = &(pResultBytes[0]);
        const auto& lVal = &(pValueBytes[0]);
        for (size_t i = 0, j = elemSize; i < elemSize && j > 0; i++, j--)
        {
            CHECK(lRes[i] == lVal[j - 1]);
        }
    }
    {
        const auto& lRes = &(pResultBytes[6]);
        const auto& lVal = &(pValueBytes[6]);
        for (size_t i = 0, j = elemSize; i < elemSize && j > 0; i++, j--)
        {
            CHECK(lRes[i] == lVal[j - 1]);
        }
    }
    sys::byteSwap(swappedValues.data(), elemSize, numElements); // swap back
    for (size_t i = 0; i < swappedValues.size(); i++)
    {
        CHECK(pResultBytes[i] == pValueBytes[i]);
    }
}

template <typename T>
static inline void six_byteSwap(const void* in, T& out)
{
    auto const inBytes = sys::make_span<std::byte>(in, sizeof(T));
    out = sys::byteSwapValue<T>(inBytes);
}
TEST_CASE("testSixByteSwap")
{
    const int i = 123;
    int i_swapped;
    six_byteSwap(&i, i_swapped);
    CHECK(i != i_swapped);

    int result;
    six_byteSwap(&i_swapped, result);
    CHECK(i == result);
}