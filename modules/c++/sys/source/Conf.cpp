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

#include <stdexcept>
#include <type_traits>

#include "sys/Conf.h"
#include "coda_oss/bit.h"

using endian = coda_oss::endian;

// https://en.cppreference.com/w/cpp/types/endian
inline bool is_big_endian()
{
    auto endianness = endian::native; // "conditional expression is constant"
    if (endianness == endian::big)
    {
        return true;
    }
    if (endianness == endian::little)
    {
        return false;
    }
    throw std::logic_error("Mixed-endian not supported.");
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
void sys::byteSwap(void* buffer, unsigned short elemSize, size_t numElems)
{
    sys::byte* bufferPtr = static_cast<sys::byte*>(buffer);
    if (!bufferPtr || elemSize < 2 || !numElems)
        return;

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
void sys::byteSwap(const void* buffer,
                     unsigned short elemSize,
                     size_t numElems,
                     void* outputBuffer)
{
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