#ifndef CODA_OSS_sys_ByteSwap_h_INCLUDED_
#define CODA_OSS_sys_ByteSwap_h_INCLUDED_
#pragma once

#include <memory>

#include <mt/ThreadPlanner.h>
#include <mt/ThreadGroup.h>

#include "Conf.h"

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

/*
 * Threaded byte-swapping
 *
 * \param buffer Buffer to swap (contents will be overridden)
 * \param elemSize Size of each element in 'buffer'
 * \param numElements Number of elements in 'buffer'
 * \param numThreads Number of threads to use for byte-swapping
 */
template <typename T, typename U = T>
void threadedByteSwap(const T* buffer,
                             size_t elemSize,
                             size_t numElements,
                             size_t numThreads,
                             U* outputBuffer);
template <typename T>
inline void threadedByteSwap(T* buffer,
              size_t elemSize,
              size_t numElements,
              size_t numThreads)
{
    threadedByteSwap(buffer, elemSize, numElements, numThreads, buffer);
}

/*
 * Threaded byte-swapping and copy
 *
 * \param buffer Buffer to swap
 * \param elemSize Size of each element in 'buffer'
 * \param numElements Number of elements in 'buffer'
 * \param numThreads Number of threads to use for byte-swapping
 * \param outputBuffer buffer to write into
 */
template <typename T, typename U>
inline void threadedByteSwap(const T* buffer,
              size_t elemSize,
              size_t numElements,
              size_t numThreads,
              U* outputBuffer)
{
    if (numThreads <= 1)
    {
        sys::byteSwap(buffer, elemSize, numElements, outputBuffer);
        return;
    }

    mt::ThreadGroup threads;
    const mt::ThreadPlanner planner(numElements, numThreads);

    size_t threadNum(0);
    size_t startElement(0);
    size_t numElementsThisThread(0);
    while (planner.getThreadInfo(threadNum++, startElement, numElementsThisThread))
    {
        auto thread = std::make_unique<ByteSwapCopyRunnable<T, U>>(
                buffer,
                elemSize,
                startElement,
                numElementsThisThread,
                outputBuffer);

        threads.createThread(thread.release());
    }
    threads.joinAll();
}
}

#endif  // CODA_OSS_sys_ByteSwap_h_INCLUDED_