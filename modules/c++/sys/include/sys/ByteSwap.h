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
        mBuffer(buffer + startElement),
        mElemSize(elemSize),
        mNumElements(numElements),
        mOutputBuffer(outputBuffer + startElement)
    {
    }

    virtual ~ByteSwapCopyRunnable() = default;
    ByteSwapCopyRunnable(const ByteSwapCopyRunnable&) = delete;
    ByteSwapCopyRunnable& operator=(const ByteSwapCopyRunnable&) = delete;
    ByteSwapCopyRunnable(ByteSwapCopyRunnable&&) = default;
    ByteSwapCopyRunnable& operator=(ByteSwapCopyRunnable&&) = default;

    void run() override
    {
        sys::byteSwap(mBuffer, mElemSize, mNumElements, mOutputBuffer);
    }

private:
    const T* const mBuffer;
    const size_t mElemSize;
    const size_t mNumElements;
    U* const mOutputBuffer;
};

}

#endif  // CODA_OSS_sys_ByteSwap_h_INCLUDED_
