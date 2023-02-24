/* =========================================================================
 * This file is part of mem-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2020, Radiant Geospatial Solutions
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
#ifndef __MEM_CIRCULAR_BUFFER_H__
#define __MEM_CIRCULAR_BUFFER_H__

#include <vector>
#include <sys/Conf.h>

namespace mem
{

/*! CircularBuffer
 * Simple fixed-size circular buffer allowing wrapping writes, reads, 
 * indexing over a templated vector.
 */
template<typename DataT>
class CircularBuffer
{
public:

    /*! Ctor
     * Allocates sizeof(DataT)*maxNumElts and initializes internal data structures.
     * \param maxNumElts number of DataTs you want to be able to store at once
     * \throws except::Exception if maxNumElts is zero
     */
    CircularBuffer(size_t maxNumElts)
        : mBuffer(maxNumElts),
          mRead(0),
          mWrite(0),
          mAtCapacity(false)
    {
        if (!maxNumElts)
        {
            throw except::Exception(Ctxt("Attempted to create an empty CircularBuffer"));
        }
    }

    /*! Writes data into the circular buffer. May throw if overwriting unread 
     * data. Will not modify underlying buffer in the event of that throw.
     * \param buffer Pointer to DataT buffer to write
     * \param numElts Number of DataT in buffer to write
     * \throws except::Exception if trying to overwrite undread data
     */
    void write(DataT* buffer, size_t numElts)
    {
        // Check for a wrapped write
        if (mWrite + numElts > mBuffer.size())
        {
            // Decompose a wrapped write into two trivial writes
            size_t beforeWrap = mBuffer.size() - mWrite;
            size_t afterWrap = numElts - beforeWrap;

            // Check for a post-wrap overrun before doing anything to preserve
            // current state
            if (mRead < afterWrap)
            {
                throw except::Exception(Ctxt("CircularBuffer write() overrun"));   
            }

            write(buffer, beforeWrap);
            write(buffer + beforeWrap, afterWrap);
        }
        // Check for an overrun
        else if (((mWrite < mRead) && (mWrite + numElts > mRead)) || mAtCapacity)
        {
            throw except::Exception(Ctxt("CircularBuffer write() overrun"));
        }
        // This should be a trivial write
        else
        {
            memcpy(mBuffer.data() + mWrite, buffer, numElts*sizeof(DataT));
            mWrite = (mWrite + numElts) % mBuffer.size();
            if (mWrite == mRead)
            {
                mAtCapacity = true;
            }
        }
    }

    /*! Reads data from the circular buffer. May throw if enough data hasn't
     * been written yet. Will not modify the provided buffer in the event of
     * that throw
     * \param buffer Pointer to DataT buffer to read data into
     * \param numElts Number of elements to read from the circular buffer
     * \returns numElts
     * \throws except::Exception if trying to read more data than is available
     */
    size_t read(DataT* buffer, size_t numElts)
    {
        // Check for a wrapped read
        if (mRead + numElts > mBuffer.size())
        {
            // Decompose into two trivial reads...
            size_t beforeWrap = mBuffer.size() - mRead;
            size_t afterWrap = numElts - beforeWrap;

            if (mWrite < afterWrap)
            {
                throw except::Exception(Ctxt("CircularBuffer read() overrun"));
            }
            
            size_t actuallyRead(0);
            actuallyRead += read(buffer, beforeWrap);
            actuallyRead += read(buffer + beforeWrap, afterWrap);
            return actuallyRead;
        }
        // Check for an overrun
        else if (((mRead < mWrite) && (mRead + numElts > mWrite)) || (mWrite == mRead && !mAtCapacity))
        {
            throw except::Exception(Ctxt("CircularBuffer read() overrun"));
        }
        else
        {
            memcpy(buffer, mBuffer.data() + mRead, numElts*sizeof(DataT));
            mRead = (mRead + numElts) % mBuffer.size();
            mAtCapacity = false;
            return numElts;
        }
    }

    /*! Simple index operation, retrieves idx % size, no guarantees what's there,
     * may be uninitialized if data hasn't been fully written yet.
     * \param idx
     * \returns Reference to a DataT
     */
    DataT& operator[](size_t idx) 
    {
        return mBuffer[idx % mBuffer.size()];
    }

    /*! Simple index operation, retrieves idx % size, no guarantees what's there,
     * may be uninitialized if data hasn't been fully written yet.
     * \param idx
     * \return Const ref to a DataT
     */
    const DataT& operator[](size_t idx) const
    {
        return (*this)[idx];
    }

    /*! Returns the number of elements available for reading
     * \returns size_t number of DataT available to read()
     */
    size_t availableForRead() const
    {
        if (mWrite == mRead)
        {
            return mAtCapacity ? mBuffer.size() : 0;
        }
        return (mWrite > mRead) ? (mWrite - mRead) : (mBuffer.size() - mRead + mWrite);
    }

    /*! Returns the number of elements available to write before overruning 
     * the read pointer
     * \returns size_t number of Data available to write()
     */
    size_t availableForWrite() const
    {
        if (mWrite == mRead)
        {
            return mAtCapacity ? 0 : mBuffer.size();
        }
        return (mRead > mWrite) ? (mRead - mWrite) : (mBuffer.size() - mWrite + mRead);
    }

private:
    std::vector<DataT> mBuffer;
    size_t mRead;
    size_t mWrite;
    bool mAtCapacity; // Need this to tell apart a full buffer from an empty buffer when mRead == mWrite
};

}// namespace mem

#endif
