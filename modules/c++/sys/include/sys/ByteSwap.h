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
    ByteSwapCopyRunnable() = delete;
    ByteSwapCopyRunnable(coda_oss::span<const T> buffer, coda_oss::span<U> outputBuffer) :
        mBuffer(buffer), mOutputBuffer(outputBuffer)
    {
    }
    virtual ~ByteSwapCopyRunnable() = default;
    ByteSwapCopyRunnable(const ByteSwapCopyRunnable&) = delete;
    ByteSwapCopyRunnable& operator=(const ByteSwapCopyRunnable&) = delete;
    ByteSwapCopyRunnable(ByteSwapCopyRunnable&&) = default;
    ByteSwapCopyRunnable& operator=(ByteSwapCopyRunnable&&) = default;

    void run() override
    {
        sys::byteSwap(mBuffer, mOutputBuffer);
    }

private:
    const coda_oss::span<const T> mBuffer;
    const coda_oss::span<U> mOutputBuffer;
};

/*
 * Threaded byte-swapping and copy
 *
 * \param buffer Buffer to swap
 * \param numThreads Number of threads to use for byte-swapping
 * \param outputBuffer buffer to write into
 */
template <typename T, typename U = T>
inline void byteSwap(coda_oss::span<const T> buffer,
                     size_t numThreads,
                     coda_oss::span<U> outputBuffer)
{
    if (numThreads <= 1)
    {
        sys::byteSwap(buffer, outputBuffer);
        return;
    }

    mt::ThreadGroup threads;
    const mt::ThreadPlanner planner(buffer.size(), numThreads);

    size_t threadNum(0);
    size_t startElement(0);
    size_t numElementsThisThread(0);
    while (planner.getThreadInfo(threadNum++, startElement, numElementsThisThread))
    {
        const coda_oss::span<const T> buffer_(buffer.data() + startElement, numElementsThisThread);
        const coda_oss::span<T> outputBuffer_(outputBuffer.data() + startElement, numElementsThisThread);

        auto thread = std::make_unique<ByteSwapCopyRunnable<T, U>>(buffer_, outputBuffer_);
        threads.createThread(thread.release());
    }
    threads.joinAll();
}
}

#endif  // CODA_OSS_sys_ByteSwap_h_INCLUDED_
