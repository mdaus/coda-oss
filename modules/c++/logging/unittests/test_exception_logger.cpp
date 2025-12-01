/* =========================================================================
 * This file is part of logging-c++
 * =========================================================================
 *
 * (C) Copyright 2013 - 2014, MDA Information Systems LLC
 *
 * logging-c++ is free software; you can redistribute it and/or modify
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

#include <vector>
#include <memory>
#include <span>

#include <catch2/catch_test_macros.hpp>

#include <sys/Mutex.h>
#include <mem/SharedPtr.h>
#include <mt/GenerationThreadPool.h>
#include <logging/ExceptionLogger.h>

class RunNothing final : public sys::Runnable
{
    size_t& counter;
    logging::ExceptionLogger* exLog;
    bool getBacktrace;

    static sys::Mutex* counterLock()
    {
        static sys::Mutex lock;
        return &lock;
    }

public:
    RunNothing(size_t& c, logging::ExceptionLogger* el, bool getBacktrace_=false) : counter(c), exLog(el), getBacktrace(getBacktrace_) {}

    virtual void run() override
    {
        if(exLog->hasLogged())
            return;
       
        {
            mt::CriticalSection<sys::Mutex> crit(counterLock());
            counter++;
        }

        if (getBacktrace)
            exLog->log(except::Exception("Bad run").backtrace(), logging::LogLevel::LOG_ERROR);
        else
            exLog->log(except::Exception("Bad run"), logging::LogLevel::LOG_ERROR);
    }
};

TEST_CASE("testExceptionLogger")
{
    logging::Logger log("test");

    logging::ExceptionLogger exLog(&log);

    size_t counter(0);
    uint16_t numThreads(2);
 
    std::vector<sys::Runnable*> runs;
   
    mt::GenerationThreadPool pool(numThreads);
    pool.start();

    runs.push_back(new RunNothing(counter, &exLog));
    pool.addAndWaitGroup(runs);
    runs.clear();

    runs.push_back(new RunNothing(counter, &exLog));
    pool.addAndWaitGroup(runs);
    runs.clear();

    CHECK(counter == 1);
}

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4702)  // unreachable code
#endif
TEST_CASE("testExceptionWithBacktrace")
{
    const std::string getBacktrace("***** getBacktrace() *****");
    std::string s, what;
    try
    {
        throw except::Exception("Bad run");
        FAIL();
    }
    catch (const except::Throwable& t)
    {
        CHECK(std::ssize(t.getBacktrace()) == 0);
        s = t.toString();
        what = t.what();
    }
    CHECK(!s.empty());
    CHECK(!what.empty());
    CHECK(s != what);
    auto getBacktrace_pos = s.find(getBacktrace);
    CHECK(getBacktrace_pos == std::string::npos);
    getBacktrace_pos = what.find(getBacktrace);
    CHECK(getBacktrace_pos != std::string::npos);
    
    try
    {
        throw except::Exception("Bad run").backtrace();
        FAIL();
    }
    catch (const except::Throwable& t)
    {
        const auto backtraceSize = static_cast<int64_t>(t.getBacktrace().size());
        CHECK(backtraceSize > 0);
        s = t.toString(true /*includeBacktrace*/);
        what = t.what();
    }

    CHECK(!s.empty());
    CHECK(!what.empty());
    CHECK(s == what);
    getBacktrace_pos = s.find(getBacktrace);
    CHECK(getBacktrace_pos != std::string::npos);
    getBacktrace_pos = what.find(getBacktrace);
    CHECK(getBacktrace_pos != std::string::npos);
}
#ifdef _MSC_VER
#pragma warning(pop)
#endif

