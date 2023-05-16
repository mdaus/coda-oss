#ifndef CODA_OSS_mt_ByteSwap_h_INCLUDED_
#define CODA_OSS_mt_ByteSwap_h_INCLUDED_
#pragma once

#include <memory>

#include "sys/Conf.h"
#include "sys/Runnable.h"

#include "ThreadPlanner.h"
#include "ThreadGroup.h"

namespace mt
{
struct ByteSwapCopyRunnable final : public sys::Runnable
{
    ByteSwapCopyRunnable(const void* buffer,
                         size_t elemSize,
                         size_t startElement,
                         size_t numElements,
                         void* outputBuffer) :
        mBuffer(static_cast<const sys::byte*>(buffer) + startElement * elemSize),
        mElemSize(static_cast<unsigned short>(elemSize)),
        mNumElements(numElements),
        mOutputBuffer(static_cast<sys::byte*>(outputBuffer) + startElement * elemSize)
    {
    }

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
 * Threaded byte-swapping and copy
 *
 * \param buffer Buffer to swap
 * \param elemSize Size of each element in 'buffer'
 * \param numElements Number of elements in 'buffer'
 * \param numThreads Number of threads to use for byte-swapping
 * \param outputBuffer buffer to write into
 */
inline
void threadedByteSwap(const void* buffer,
              size_t elemSize,
              size_t numElements,
              size_t numThreads,
              void* outputBuffer)
{
    if (numThreads <= 1)
    {
        sys::byteSwapV(buffer,
                      static_cast<unsigned short>(elemSize),
                      numElements,
                      outputBuffer);
        return;
    }

    mt::ThreadGroup threads;
    const mt::ThreadPlanner planner(numElements, numThreads);

    size_t threadNum(0);
    size_t startElement(0);
    size_t numElementsThisThread(0);
    while (planner.getThreadInfo(threadNum++, startElement, numElementsThisThread))
    {
        auto thread = std::make_unique<ByteSwapCopyRunnable>(
                buffer,
                elemSize,
                startElement,
                numElementsThisThread,
                outputBuffer);
        threads.createThread(thread.release());
    }
    threads.joinAll();
}

/*
 * Threaded byte-swapping
 *
 * \param buffer Buffer to swap (contents will be overridden)
 * \param elemSize Size of each element in 'buffer'
 * \param numElements Number of elements in 'buffer'
 * \param numThreads Number of threads to use for byte-swapping
 */
inline
void threadedByteSwap(void* buffer,
              size_t elemSize,
              size_t numElements,
              size_t numThreads)
{
    threadedByteSwap(buffer, elemSize, numElements, numThreads, buffer);
}

}

#endif  // CODA_OSS_mt_ByteSwap_h_INCLUDED_
