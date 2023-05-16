#ifndef CODA_OSS_mt_ByteSwap_h_INCLUDED_
#define CODA_OSS_mt_ByteSwap_h_INCLUDED_
#pragma once

#include <memory>
#include <stdexcept>

#include "coda_oss/span.h"
#include "sys/Conf.h"
#include "sys/ByteSwap.h"

#include "ThreadPlanner.h"
#include "ThreadGroup.h"

namespace mt
{
/*
 * Threaded byte-swapping and copy
 *
 * \param buffer Buffer to swap
 * \param elemSize Size of each element in 'buffer'
 * \param numElements Number of elements in 'buffer'
 * \param numThreads Number of threads to use for byte-swapping
 * \param outputBuffer buffer to write into
 */
template<typename T, typename U = T>
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
        auto thread = std::make_unique<sys::ByteSwapCopyRunnable<T, U>>(
                buffer,
                elemSize,
                startElement,
                numElementsThisThread,
                outputBuffer);
        threads.createThread(thread.release());
    }
    threads.joinAll();
}
template<typename T, typename U = T>
inline void threadedByteSwap(coda_oss::span<const T> buffer, size_t numThreads, coda_oss::span<U> outputBuffer)
{
    // outputBuffer could be std::byte
    if (buffer.size_bytes() != outputBuffer.size_bytes())
    {
        throw std::invalid_argument("buffer.size_bytes() != outputBuffer.size_bytes()");
    }
    threadedByteSwap(buffer.data(), sizeof(T), buffer.size(), numThreads, outputBuffer.data());
}

/*
 * Threaded byte-swapping
 *
 * \param buffer Buffer to swap (contents will be overridden)
 * \param elemSize Size of each element in 'buffer'
 * \param numElements Number of elements in 'buffer'
 * \param numThreads Number of threads to use for byte-swapping
 */
template <typename T>
inline void threadedByteSwap(T* buffer,
              size_t elemSize,
              size_t numElements,
              size_t numThreads)
{
    threadedByteSwap(buffer, elemSize, numElements, numThreads, buffer);
}
template <typename T>
inline void threadedByteSwap(coda_oss::span<T> buffer, size_t numThreads)
{
    threadedByteSwap(buffer.data(), sizeof(T), buffer.size(), numThreads);
}

}

#endif  // CODA_OSS_mt_ByteSwap_h_INCLUDED_
