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

#ifndef CODA_OSS_sys_ByteSwapValue_h_INCLUDED_
#define CODA_OSS_sys_ByteSwapValue_h_INCLUDED_
#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

#include <new>
#include <type_traits>
#include <coda_oss/span.h>
#include <coda_oss/cstddef.h>
#include <coda_oss/bit.h>
#include <tuple>
#include <vector>
#include <array>
#include <stdexcept>

#include "config/Exports.h"

#include "Span.h"

namespace sys
{
    namespace details
    {
    /*!
     * Swap bytes for a single value into output buffer.  API is `span<byte>` rather than `void*` since
     * we often know the size of the buffers.  These "low level" routines may be less efficient than
     * the templates since it's not possible to specialize on a specific size.
     *
     *  \param buffer to transform
     *  \param[out] outputBuffer buffer to write swapped elements to
     */
    coda_oss::span<const coda_oss::byte> CODA_OSS_API swapValueBytes(
        coda_oss::span<const coda_oss::byte> pIn, coda_oss::span<coda_oss::byte> outPtr, std::nothrow_t) noexcept;
    coda_oss::span<const coda_oss::byte> CODA_OSS_API swapValueBytes(const void* pIn, size_t elemSize, void* outPtr) noexcept;
    coda_oss::span<const coda_oss::byte> CODA_OSS_API swapValueBytes(
        coda_oss::span<const coda_oss::byte> pIn, coda_oss::span<coda_oss::byte> outPtr);

    template <typename TUInt>
    inline auto swapUIntBytes_(const TUInt* pIn, TUInt* pOut) noexcept
    {
        static_assert(std::is_unsigned<TUInt>::value, "TUInt must be 'unsigned'");
        *pOut = coda_oss::byteswap(*pIn);

        // Give the raw byte-swapped bytes back to the caller for easy serialization
        return as_bytes(pOut);
    }
    template <typename TUInt>
    inline auto swapUIntBytes(coda_oss::span<const coda_oss::byte> pInBytes, coda_oss::span<coda_oss::byte> pOutBytes,
        std::nothrow_t) noexcept
    {
        assert(sizeof(TUInt) == pInBytes.size());
        assert(pInBytes.size() == pOutBytes.size());

        const void* const pIn_ = pInBytes.data();
        auto const pIn = static_cast<const TUInt*>(pIn_);
        void* pOut_ = pOutBytes.data();
        auto const pOut = static_cast<TUInt*>(pOut_);
        return swapUIntBytes_(pIn, pOut);
    }
    template <typename TUInt>
    inline auto swapUIntBytes(coda_oss::span<const coda_oss::byte> pInBytes, coda_oss::span<coda_oss::byte> pOutBytes)
    {
        if (sizeof(TUInt) != pInBytes.size())
        {
            throw std::invalid_argument("'pInBytes.size() != sizeof(TUInt)");
        }
        if (pInBytes.size() != pOutBytes.size())
        {
            throw std::invalid_argument("'pInBytes.size() != pOutBytes.size()");
        }
        return swapUIntBytes<TUInt>(pInBytes, pOutBytes, std::nothrow);
    }
    template <typename TUInt>
    inline auto swapUIntBytes(const void* pIn_, coda_oss::span<coda_oss::byte> pOutBytes)
    {
        if (sizeof(TUInt) != pOutBytes.size())
        {
            throw std::invalid_argument("'pOutBytes.size() != sizeof(TUInt)");
        }
        auto const pInBytes = as_bytes(static_cast<const TUInt*>(pIn_));
        return swapUIntBytes<TUInt>(pInBytes, pOutBytes);
    }

    // This is a template so that we can have specializations for different sizes
    template <size_t elemSize>
    inline auto swapBytes(const void* pIn, coda_oss::span<coda_oss::byte> outPtr)
    {
        auto const inPtr = make_span<coda_oss::byte>(pIn, elemSize);
        return swapValueBytes(inPtr, outPtr);
    }
    template <>
    inline auto swapBytes<sizeof(uint8_t)>(const void* pIn, coda_oss::span<coda_oss::byte> pOut)
    {
        return swapUIntBytes<uint8_t>(pIn, pOut);
    }
    template <>
    inline auto swapBytes<sizeof(uint16_t)>(const void* pIn, coda_oss::span<coda_oss::byte> pOut)
    {
        return swapUIntBytes<uint16_t>(pIn, pOut);
    }
    template <>
    inline auto swapBytes<sizeof(uint32_t)>(const void* pIn, coda_oss::span<coda_oss::byte> pOut)
    {
        return swapUIntBytes<uint32_t>(pIn, pOut);
    }
    template <>
    inline auto swapBytes<sizeof(uint64_t)>(const void* pIn, coda_oss::span<coda_oss::byte> pOut)
    {
        return swapUIntBytes<uint64_t>(pIn, pOut);
    }

    template<typename T>
    inline constexpr bool is_byte_swappable() noexcept
    {
        // Trying to byte-swap anything other than integers is likely to cause
        // problems (or at least confusion):
        // * `struct`s have padding that should be ignored.
        // * each individual member of a `struct` should be byte-swaped
        // * byte-swaped `float` or `double` bits are nonsense
        return !std::is_compound<T>::value;
    }
    }

    /*!
    * Function to swap one element irrespective of size.
    * Returns the raw byte-swapped bytes for easy serialization.
    */
    template <typename T>
    inline auto swapBytes(const T* pIn, coda_oss::span<coda_oss::byte> pOut)
    {
        if (sizeof(T) != pOut.size())
        {
            throw std::invalid_argument("'pOut.size() != sizeof(T)");
        }
        static_assert(details::is_byte_swappable<T>(), "T should not be a 'struct'");
        return details::swapBytes<sizeof(T)>(pIn, pOut);
    }
    template <typename T>
    inline coda_oss::span<const coda_oss::byte> swapBytes(const T* pIn, void* pOut)
    {
        auto const pBytes = make_span<coda_oss::byte>(pOut, sizeof(T));
        return swapBytes(pIn, pBytes);
    }
    template <typename T>
    inline auto swapBytes(T in, std::array<coda_oss::byte, sizeof(T)>& out)
    {
        return swapBytes(&in, make_span(out));
    }
    template <typename T>
    inline auto swapBytes(T in, std::vector<coda_oss::byte>& out)
    {
        out.resize(sizeof(T));
        return swapBytes(&in, make_span(out));
    }
    template <typename T>
    inline auto swapBytes(T in)
    {
        std::vector<coda_oss::byte> retval;
        std::ignore = swapBytes(in, retval);
        return retval;
    }

    // Reverse the above: turn `span<byte>` back to T after byte-swapping
    template <typename T>
    inline auto swapBytes(coda_oss::span<const coda_oss::byte> in)
    {
        if (sizeof(T) != in.size())
        {
            throw std::invalid_argument("'in.size() != sizeof(T)");
        }
        static_assert(details::is_byte_swappable<T>(), "T should not be a 'struct'"); // see above

        // Don't want to cast the swapped bytes in `in` to T* as they might not be valid;
        // e.g., a byte-swapped `float` could be garbage.
        T retval;
        details::swapBytes<sizeof(T)>(in, as_bytes(retval));
        return retval;
    }
    template <typename T>
    inline auto swapBytes(const std::array<coda_oss::byte, sizeof(T)>& in)
    {
        return swapBytes<T>(make_span(in));
    }
    template <typename T>
    inline auto swapBytes(const std::vector<coda_oss::byte>& in)
    {
        return swapBytes<T>(make_span(in));
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
    template <typename T> inline T byteSwap(T val)
    {
        T out;
        std::ignore = swapBytes(&val, &out);
        return out;
    }
}
#endif // CODA_OSS_sys_ByteSwapValue_h_INCLUDED_
