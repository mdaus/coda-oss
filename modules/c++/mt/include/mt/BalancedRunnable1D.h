/* =========================================================================
 * This file is part of mt-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2017, MDA Information Systems LLC
 *
 * mt-c++ is free software; you can redistribute it and/or modify
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
#ifndef __MT_BALANCED_RUNNABLE_1D_H__
#define __MT_BALANCED_RUNNABLE_1D_H__

#include <vector>
#include <sstream>

#include <sys/Conf.h>
#include <sys/Runnable.h>
#include <except/Exception.h>
#include <mt/ThreadPlanner.h>
#include <mt/ThreadGroup.h>

namespace mt
{
/*!
 *  Given a reference to an atomic counter, this runnable will atomically get
 *  and then increment an element, passing the fetched element to
 *  the provided functor for processing. Each runnable will operate over the
 *  full range of elements.
 *
 *  This runnable is useful in cases where work needs to be
 *  done across a range of elements, but when dividing these elements
 *  across threads leads to balancing issues i.e certain threads
 *  terminate earlier than other threads.
 *
 *  \param[in,out] atomicCounter Atomic counter all threads will use to
 *  fetch elements to process
 *
 *  \param numElements The global number of elements to process
 *
 *  \param op Functor to use
 *
 */
template <typename OpT>
class BalancedRunnable1D : public sys::Runnable
{
public:
        BalancedRunnable1D(size_t numElements,
                           sys::AtomicCounter& atomicCounter,
                           const OpT& op) :
        mNumElements(numElements),
        mCounter(atomicCounter),
        mOp(op)
    {
    }

    virtual void run()
    {
        while (true)
        {
            const size_t element = mCounter.getThenIncrement();
            if (element < mNumElements)
            {
                mOp(element);
            }
            else
            {
                break;
            }
        }
    }

private:
    const size_t mNumElements;
    sys::AtomicCounter& mCounter;
    const OpT& mOp;
};

/*!
 *  For each thread, runs a BalancedRunnable1D object using the provided
 *  functor.
 *
 *  \param numElements Number of elements of work
 *  \param numThreads Number of threads that will run BalancedRunnable1D
 *  \param op Functor to use
 */
template <typename OpT>
void runBalanced1D(size_t numElements,
                   size_t numThreads,
                   const OpT& op)
{
    sys::AtomicCounter counter(0);
    if (numThreads <= 1)
    {
        BalancedRunnable1D<OpT>(numElements, counter, op).run();
    }
    else
    {
        ThreadGroup threads;
        for (size_t ii = 0; ii < numThreads; ++ii)
        {
            threads.createThread(new BalancedRunnable1D<OpT>(
                     numElements, counter, op));
        }
        threads.joinAll();
    }
}

/*!
 *  Same as above, but instead of sharing a functor across runnables,
 *  each runnable will receive its own.
 *
 *  \param numElements Number of elements of work
 *  \param numThreads Number of threads that will run BalancedRunnable1D
 *  \param ops Vector of functors to use
 */
template <typename OpT>
void runBalanced1D(size_t numElements,
                   size_t numThreads,
                   const std::vector<OpT>& ops)
{
    sys::AtomicCounter counter(0);
    if (ops.size() != numThreads)
    {
        std::ostringstream ostr;
        ostr << "Got " << numThreads << " threads but " << ops.size()
             << " functors";
        throw except::Exception(Ctxt(ostr.str()));
    }

    if (numThreads <= 1)
    {
        BalancedRunnable1D<OpT>(numElements, counter, ops[0]).run();
    }
    else
    {
        ThreadGroup threads;
        for (size_t ii = 0; ii < numThreads; ++ii)
        {
            threads.createThread(new BalancedRunnable1D<OpT>(
                    numElements, counter, ops[ii]));
        }

        threads.joinAll();
    }
}

/*!
 *  Convenience wrapper for providing each runnable with a copy of op.
 *  This is useful in cases where each BalancedRunnable1D should use a
 *  functor with its own local storage.
 *
 *  \param numElements Number of elements of work
 *  \param numThreads Number of threads that will run BalancedRunnable1D
 *  \param op Functor to use
 */
template <typename OpT>
void runBalanced1DWithCopies(size_t numElements,
                             size_t numThreads,
                             const OpT& op)
{
    const std::vector<OpT> ops(numThreads, op);
    runBalanced1D(numElements, numThreads, ops);
}
}

#endif
