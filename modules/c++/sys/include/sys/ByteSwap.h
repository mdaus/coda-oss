/* =========================================================================
 * This file is part of sys-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * (C) Copyright 2021, Maxar Technologies, Inc.
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

#ifndef CODA_OSS_sys_ByteSwap_h_INCLUDED_
#define CODA_OSS_sys_ByteSwap_h_INCLUDED_
#pragma once

#include <stdint.h>
#include <stdlib.h>

#include "config/Exports.h"

namespace sys
{
   /*!
     *  Swap bytes in-place.  Note that a complex pixel
     *  is equivalent to two floats so elemSize and numElems
     *  must be adjusted accordingly.
     *
     *  \param [inout] buffer to transform
     *  \param elemSize
     *  \param numElems
     */
    void CODA_OSS_API byteSwap_(void* buffer, size_t elemSize, size_t numElems);
    inline void byteSwap(void* buffer,
                         unsigned short elemSize,
                         size_t numElems)
    {
        byteSwap_(buffer, elemSize, numElems);
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
    void CODA_OSS_API byteSwap_(const void* buffer, size_t elemSize, size_t numElems, void* outputBuffer);
    inline void  byteSwap(const void* buffer,
                          unsigned short elemSize,
                          size_t numElems,
                          void* outputBuffer)
    {
        byteSwap_(buffer, elemSize, numElems, outputBuffer);
    }

    /*!
     *  Function to swap one element irrespective of size.  The inplace
     *  buffer function should be preferred.
     *
     *  To specialize complex float, first include the complex library
     *  \code
        #include <complex>
     *  \endcode
     *
     *  Then put an overload in as specified below:
     *  \code
        template <typename T> std::complex<T> byteSwap(std::complex<T> val)
        {
            std::complex<T> out(byteSwap<T>(val.real()),
                                byteSwap<T>(val.imag()));
            return out;
        }
     *  \endcode
     *
     */
    template <typename T> T byteSwap(T val)
    {
        size_t size = sizeof(T);
        T out;

        unsigned char* cOut = reinterpret_cast<unsigned char*>(&out);
        unsigned char* cIn = reinterpret_cast<unsigned char*>(&val);
        for (size_t i = 0, j = size - 1; i < j; ++i, --j)
        {
            cOut[i] = cIn[j];
            cOut[j] = cIn[i];
        }
        return out;
    }
}
#endif // CODA_OSS_sys_ByteSwap_h_INCLUDED_
