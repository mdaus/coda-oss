/* =========================================================================
 * This file is part of mt-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

#include "import/sys.h"
#include "import/mt.h"
#include <catch2/catch_test_macros.hpp>

struct MyRunTask final : public sys::Runnable
{
    int result;
    int *state;
    int *num_deleted;
    
    MyRunTask(int *new_state, int *new_num_deleted)
    {
        state = new_state;
        result = *new_state;
        num_deleted = new_num_deleted;
    }
    virtual ~MyRunTask()
    {
        (*num_deleted)++;
    }

    virtual void run() override
    {
		while (result == 1)
            result = *state;
    }
};

TEST_CASE("DoThreadGroupTest")
{
    auto threads = new mt::ThreadGroup();
    int state = 1, numDeleted = 0;
    MyRunTask *tasks[3];
    
    for (int i = 0; i < 3; i++)
        tasks[i] = new MyRunTask(&state, &numDeleted);
    
    threads->createThread(tasks[0]);
    threads->createThread(tasks[1]);
    state = 2;
    threads->joinAll();
    
    CHECK(tasks[0]->result == 2);
    CHECK(tasks[1]->result == 2);
    
    state = 1;
    threads->createThread(tasks[2]);
    state = 3;

    CHECK(numDeleted == 0);

    delete threads;
    CHECK(numDeleted == 3);
}

TEST_CASE("PinToCPUTest")
{
    bool defaultValue;
#if defined(MT_DEFAULT_PINNING)
    defaultValue = true;
#else
    defaultValue = false;
#endif
    // Check the pinning settings for the default value
    CHECK(mt::ThreadGroup::getDefaultPinToCPU() == defaultValue);
    mt::ThreadGroup threads1;
    CHECK(threads1.isPinToCPUEnabled() == defaultValue);

    // Check the pinning settings when pinning is enabled
    mt::ThreadGroup::setDefaultPinToCPU(true);
    CHECK(mt::ThreadGroup::getDefaultPinToCPU());
    mt::ThreadGroup threads2;
    CHECK(threads2.isPinToCPUEnabled());
   
    // Check the pinning settings when pinning is disabled
    mt::ThreadGroup::setDefaultPinToCPU(false);
    CHECK_FALSE(mt::ThreadGroup::getDefaultPinToCPU());
    mt::ThreadGroup threads3;
    CHECK_FALSE(threads3.isPinToCPUEnabled());
}
