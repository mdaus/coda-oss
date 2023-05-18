/* =========================================================================
 * This file is part of sys-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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
#include "sys/Conf.h"

#include <assert.h>

#include <stdexcept>
#include <type_traits>
#include <algorithm>
#include <tuple>
#include "coda_oss/bit.h"
#include "coda_oss/cstddef.h"
#include "coda_oss/span.h"

// https://en.cppreference.com/w/cpp/types/endian
using endian = coda_oss::endian;
template <endian endianness>
inline bool is_big_endian_()
{
    throw std::logic_error("Mixed-endian not supported.");
}
template<>
inline constexpr bool is_big_endian_<endian::big>()
{
    return true;
}
template <>
inline constexpr bool is_big_endian_<endian::little>()
{
    return false;
}
inline bool is_big_endian()
{
    return is_big_endian_<endian::native>();
}
constexpr inline bool is_big_or_little_endian()
{
    return (endian::native == endian::big) || (endian::native == endian::little) ? true : false;
}

inline bool isBigEndianSystem()
{
    // This is an endian test
    int intVal = 1;
    unsigned char* endianTest = (unsigned char*)&intVal;
    return endianTest[0] != 1;
}
bool sys::isBigEndianSystem()
{
    static_assert(is_big_or_little_endian(), "Mixed-endian not supported.");
    const auto retval = ::isBigEndianSystem();
    if (retval != is_big_endian())
    {
        throw std::logic_error("endian values don't agree!");
    }
    return retval;
}


   /*!
 *  Swap bytes in-place.  Note that a complex pixel
 *  is equivalent to two floats so elemSize and numElems
 *  must be adjusted accordingly.
 *
 *  \param [inout] buffer to transform
 *  \param elemSize
 *  \param numElems
 */
template <typename TUInt>
inline void byteSwap_n(void *buffer_, size_t elemSize, size_t numElems)
{
    static_assert(std::is_unsigned<TUInt>::value, "TUInt must be 'unsigned'");
    assert(sizeof(TUInt) == elemSize);
    std::ignore = elemSize;

    const coda_oss::span<TUInt> buffer(static_cast<TUInt*>(buffer_), numElems);
    assert(buffer.size_bytes() == elemSize * numElems);

    for (auto& v : buffer)
    {
        v = sys::byteSwap(v);
    }
}
void sys::byteSwap_(void* buffer, size_t elemSize, size_t numElems)
{
    sys::byte* bufferPtr = static_cast<sys::byte*>(buffer);
    if (!bufferPtr || elemSize < 2 || !numElems)
        return;

    if (elemSize == 2)
    {
        return byteSwap_n<uint16_t>(buffer, elemSize, numElems);
    }
    if (elemSize == 4)
    {
        return byteSwap_n<uint32_t>(buffer, elemSize, numElems);
    }
    if (elemSize == 8)
    {
        return byteSwap_n<uint64_t>(buffer, elemSize, numElems);
    }

    const auto half = elemSize >> 1;
    size_t offset = 0, innerOff = 0, innerSwap = 0;
    for (size_t i = 0; i < numElems; ++i, offset += elemSize)
    {
        for (unsigned short j = 0; j < half; ++j)
        {
            innerOff = offset + j;
            innerSwap = offset + elemSize - 1 - j;

            std::swap(bufferPtr[innerOff], bufferPtr[innerSwap]);
        }
    }
}

    /*!
 *  Swap bytes into output buffer.  Note that a complex pixel
 *  is equivalent to two floats so elemSize and numElems
 *  must be adjusted accordingly.
 *
 *  \param buffer to transform
 *  \param elemSize
 *  \param numElems
 *  \param[out] outputBuffer buffer to write swapped elements to
 */
template <typename TUInt>
inline void byteSwap_n(const void *buffer_, size_t elemSize, size_t numElems, void *outputBuffer_)
{
    static_assert(std::is_unsigned<TUInt>::value, "TUInt must be 'unsigned'");
    assert(sizeof(TUInt) == elemSize);
    std::ignore = elemSize;

    const coda_oss::span<const TUInt> buffer(static_cast<const TUInt*>(buffer_), numElems);
    assert(buffer.size_bytes() == elemSize * numElems);
    const coda_oss::span<TUInt> outputBuffer(static_cast<TUInt*>(outputBuffer_), numElems);

    std::transform(buffer.begin(), buffer.end(), outputBuffer.begin(), [](const auto& v) { return sys::byteSwap(v); });
}
void sys::byteSwap_(const void* buffer, size_t elemSize, size_t numElems, void* outputBuffer)
{
    if (!numElems || !buffer || !outputBuffer)
    {
        return;
    }

    if (elemSize == 1)
    {
        std::ignore = memcpy(outputBuffer, buffer, elemSize * numElems);
        return;
    }
    if (elemSize == 2)
    {
        return byteSwap_n<uint16_t>(buffer, elemSize, numElems, outputBuffer);
    }
    if (elemSize == 4)
    {
        return byteSwap_n<uint32_t>(buffer, elemSize, numElems, outputBuffer);
    }
    if (elemSize == 8)
    {
        return byteSwap_n<uint64_t>(buffer, elemSize, numElems, outputBuffer);
    }

    const sys::byte* bufferPtr = static_cast<const sys::byte*>(buffer);
    sys::byte* outputBufferPtr = static_cast<sys::byte*>(outputBuffer);

    if (!numElems || !bufferPtr || !outputBufferPtr)
    {
        return;
    }

    const auto half = elemSize >> 1;
    size_t offset = 0;

    for (size_t ii = 0; ii < numElems; ++ii, offset += elemSize)
    {
        for (unsigned short jj = 0; jj < half; ++jj)
        {
            const size_t innerOff = offset + jj;
            const size_t innerSwap = offset + elemSize - 1 - jj;

            outputBufferPtr[innerOff] = bufferPtr[innerSwap];
            outputBufferPtr[innerSwap] = bufferPtr[innerOff];
        }
    }
}
