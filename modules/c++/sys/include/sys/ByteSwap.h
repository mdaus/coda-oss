#ifndef __ALGS_BYTE_SWAP_H__
#define __ALGS_BYTE_SWAP_H__

#include <mt/ThreadPlanner.h>
#include <mt/ThreadGroup.h>

namespace algs
{
class ByteSwapRunnable : public sys::Runnable
{
public:
    ByteSwapRunnable(void* buffer,
                     size_t elemSize,
                     size_t startElement,
                     size_t numElements) :
        mBuffer(static_cast<sys::byte*>(buffer) + startElement * elemSize),
        mElemSize(static_cast<unsigned short>(elemSize)),
        mNumElements(numElements)
    {
    }

    virtual void run()
    {
        sys::byteSwap(mBuffer, mElemSize, mNumElements);
    }

private:
    sys::byte* const mBuffer;
    const unsigned short mElemSize;
    const size_t mNumElements;
};

class ByteSwapCopyRunnable : public sys::Runnable
{
public:
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

    virtual void run()
    {
        sys::byteSwap(mBuffer, mElemSize, mNumElements, mOutputBuffer);
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
inline
void byteSwap(void* buffer,
              size_t elemSize,
              size_t numElements,
              size_t numThreads)
{
    if (numThreads <= 1)
    {
        sys::byteSwap(buffer,
                      static_cast<unsigned short>(elemSize),
                      numElements);
    }
    else
    {
        mt::ThreadGroup threads;
        const mt::ThreadPlanner planner(numElements, numThreads);

        size_t threadNum(0);
        size_t startElement(0);
        size_t numElementsThisThread(0);
        while (planner.getThreadInfo(threadNum++,
                                     startElement,
                                     numElementsThisThread))
        {
            std::unique_ptr<sys::Runnable> thread(new ByteSwapRunnable(
                    buffer,
                    elemSize,
                    startElement,
                    numElementsThisThread));

            threads.createThread(thread.release());
        }
        threads.joinAll();
    }
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
inline
void byteSwap(const void* buffer,
              size_t elemSize,
              size_t numElements,
              size_t numThreads,
              void* outputBuffer)
{
    if (numThreads <= 1)
    {
        sys::byteSwap(buffer,
                      static_cast<unsigned short>(elemSize),
                      numElements,
                      outputBuffer);
    }
    else
    {
        mt::ThreadGroup threads;
        const mt::ThreadPlanner planner(numElements, numThreads);

        size_t threadNum(0);
        size_t startElement(0);
        size_t numElementsThisThread(0);
        while (planner.getThreadInfo(threadNum++,
                                     startElement,
                                     numElementsThisThread))
        {
            std::unique_ptr<sys::Runnable> thread(new ByteSwapCopyRunnable(
                    buffer,
                    elemSize,
                    startElement,
                    numElementsThisThread,
                    outputBuffer));

            threads.createThread(thread.release());
        }
        threads.joinAll();

    }
}
}

#endif
