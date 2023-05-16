#ifndef CODA_OSS_sys_ByteSwap_h_INCLUDED_
#define CODA_OSS_sys_ByteSwap_h_INCLUDED_
#pragma once

#include "Runnable.h"

namespace sys
{
template <typename T, typename U = T>
struct ByteSwapCopyRunnable final : public sys::Runnable
{
    ByteSwapCopyRunnable(const T* buffer,
                         size_t elemSize,
                         size_t startElement,
                         size_t numElements,
                         U* outputBuffer) :
        mBuffer(static_cast<const sys::byte*>(static_cast<const void*>(buffer)) + startElement * elemSize),
        mElemSize(static_cast<unsigned short>(elemSize)),
        mNumElements(numElements),
        mOutputBuffer(static_cast<sys::byte*>(static_cast<void*>(outputBuffer)) + startElement * elemSize)
    {
    }

    virtual ~ByteSwapCopyRunnable() = default;
    ByteSwapCopyRunnable(const ByteSwapCopyRunnable&) = delete;
    ByteSwapCopyRunnable& operator=(const ByteSwapCopyRunnable&) = delete;
    ByteSwapCopyRunnable(ByteSwapCopyRunnable&&) = default;
    ByteSwapCopyRunnable& operator=(ByteSwapCopyRunnable&&) = default;

    void run() override
    {
        sys::byteSwapV(mBuffer, mElemSize, mNumElements, mOutputBuffer);
    }

private:
    const sys::byte* const mBuffer;
    const unsigned short mElemSize;
    const size_t mNumElements;
    sys::byte* const mOutputBuffer;
};

}

#endif  // CODA_OSS_sys_ByteSwap_h_INCLUDED_
