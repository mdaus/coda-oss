/* =========================================================================
 * This file is part of mem-c++
 * =========================================================================
 *
 * (C) Copyright 2013 - 2014, MDA Information Systems LLC
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
 
#ifndef __MEM_SWAP_BUFFER_H__
#define __MEM_SWAP_BUFFER_H__

#include <algorithm>
#include <sys/Conf.h>
#include <mem/ScopedAlignedArray.h>

namespace mem
{
/*!
 *  \class SwapBuffer
 *  \brief Buffer for ping/pong processing
 *
 *  This buffer allows for users to both create data internally
 *  by specifying the number of bytes need per buffer, or the user
 *  can pass in externally created buffers. However, it's the user's
 *  responsibility to deallocate any externally created memory.
 *
 *  \note This class was intentionally created without a reset method
 *        because losing track of the state of allocation on the 
 *        current memory could result in segmentation faults. 
 *        Specifically, if a user sends in data pointers, then uses 
 *        a reset method, they could be tempted to delete the pointers
 *        returned from the get* methods, which would segfault when 
 *        the ScopedAlignedArrays go out of scope.
 */
class SwapBuffer
{
public:
    /*!
     *  Allocate the buffers to the size needed --
     *  It is suggested to allocate these buffers to the largest
     *  image segment size, because these buffers are reusable 
     *  during processing.
     *  This internally creates and manages the memory requested in a single
     *  contiguous buffer, guaranteeing both valid and scratch will match the
     *  requested alignment
     *
     *  \param numBytes Number of bytes to allocate for both valid and scratch
     *  (i.e. total allocation will be twice this value)
     *  \param alignment Byte alignment to guarantee
     */
    SwapBuffer(size_t numBytes,
               size_t alignment = sys::SSE_INSTRUCTION_ALIGNMENT) :
        mNumBytes(numBytes),
        mBuffer(numBytes +
                numPadBytesToAlignSecondBuffer(numBytes, alignment) +
                numBytes),
        mValid(mBuffer.get()),
        mScratch(static_cast<sys::byte*>(mValid) +
                 numBytes +
                 numPadBytesToAlignSecondBuffer(numBytes, alignment))
    {
    }

    /*!
     *  Pass in externally created memory --
     *  It is the responsibility of the user to deallocate any
     *  memory passed into this class.
     *
     *  \param valid Valid buffer of at least 'numBytes' bytes
     *  \param scratch Scratch buffer of at least 'numBytes' bytes
     *  \param numBytes Number of bytes in each buffer
     */
    SwapBuffer(void* valid, void* scratch, size_t numBytes) :
        mNumBytes(numBytes),
        mValid(valid),
        mScratch(scratch)
    {
    }
    
    //! \return The number of bytes
    size_t getNumBytes() const 
    { 
        return mNumBytes; 
    }

    /*!
     * \return The currently active buffer.  Users should read data from this
     * buffer during processing.
     */
    template<typename T>
    T* getValidBuffer()
    {
        return static_cast<T*>(mValid);
    }

    /*!
     * \return The currently active buffer.  Users should read data from this
     * buffer during processing.
     */
    template<typename T>
    const T* getValidBuffer() const
    {
        return static_cast<T*>(mValid);
    }

    /*!
     * \return The currently inactive buffer.  Users should write data to this
     * buffer during processing.
     */
    template<typename T>
    T* getScratchBuffer()
    {
        return static_cast<T*>(mScratch);
    }

    /*!
     * \return The currently inactive buffer.  Users should write data to this
     * buffer during processing.
     */
    template<typename T>
    const T* getScratchBuffer() const
    {
        return static_cast<T*>(mScratch);
    }

    //! Swap the buffers
    void swap() 
    {
        std::swap(mValid, mScratch);
    }

    /*!
     * \return The underlying full buffer (that both valid and scratch point
     * to).  If the constructor that tells this class to allocate the memory
     * was called, this will be a single contiguous buffer twice the size of
     * either valid or scratch.  If the other constructor was used, two
     * user-provided buffers were used so this is NULL.
     *
     * This is useful for cases where you need to write a single large buffer
     * to contiguous memory that's bigger (but not twice as big) as 'valid' and
     * you no longer need the valid/scratch ping-ponging.
     *
     * Only use this method if you *really* know what you're doing.
     */
    void* getUnderlyingBuffer()
    {
        return mBuffer.get();
    }

    /*!
     * \return The underlying full buffer (that both valid and scratch point
     * to).  If the constructor that tells this class to allocate the memory
     * was called, this will be a single contiguous buffer twice the size of
     * either valid or scratch.  If the other constructor was used, two
     * user-provided buffers were used so this is NULL.
     *
     * This is useful for cases where you need to write a single large buffer
     * to contiguous memory that's bigger (but not twice as big) as 'valid' and
     * you no longer need the valid/scratch ping-ponging.
     *
     * Only use this method if you *really* know what you're doing.
     */
    const void* getUnderlyingBuffer() const
    {
        return mBuffer.get();
    }

private:
    static
    size_t numPadBytesToAlignSecondBuffer(size_t numBytes, size_t alignment)
    {
        const size_t numLeftovers = numBytes % alignment;
        if (numLeftovers == 0)
        {
            return 0;
        }
        else
        {
            return (alignment - numLeftovers);
        }
    }

private:
    const size_t mNumBytes;

    const ScopedAlignedArray<sys::byte> mBuffer;

    void* mValid;
    void* mScratch;
};
}

#endif
