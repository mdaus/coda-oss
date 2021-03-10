/* =========================================================================
 * This file is part of sys-c++
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2016, MDA Information Systems LLC
 *
 * sys-c++ is free software; you can redistribute it and/or modify
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

#include <assert.h>

#include <sys/Path.h>
#include "TestCase.h"

namespace
{
TEST_CASE(testExpandEnvTilde)
{
    const auto result = sys::Path::expandEnvironmentVariables("~");
    TEST_ASSERT_FALSE(result.empty());
    TEST_ASSERT_TRUE(sys::Filesystem::exists(result));
    TEST_ASSERT_TRUE(sys::Filesystem::is_directory(result));
}

TEST_CASE(testExpandEnvPath)
{
    const auto path = sys::Path::expandEnvironmentVariables("$PATH", false);
    TEST_ASSERT_FALSE(path.empty());

    #if _WIN32  // %FOO% only on Windows
    const auto win32_path = sys::Path::expandEnvironmentVariables("%PATH%", false);
    TEST_ASSERT_EQ(win32_path, path);
    #endif

    const auto path2 = sys::Path::expandEnvironmentVariables("$(PATH)", false);
    TEST_ASSERT_EQ(path2, path);

    const auto path3 = sys::Path::expandEnvironmentVariables("${PATH}", false);
    TEST_ASSERT_EQ(path3, path);

    const auto foopath = sys::Path::expandEnvironmentVariables("foo${PATH}", false);
    TEST_ASSERT_EQ(foopath, "foo" + path);

    const auto pathfoo = sys::Path::expandEnvironmentVariables("${PATH}foo", false);
    TEST_ASSERT_EQ(pathfoo, path + "foo");

    const auto foopathbar = sys::Path::expandEnvironmentVariables("foo${PATH}bar", false);
    TEST_ASSERT_EQ(foopathbar, "foo" + path + "bar");

    const auto foopath_bar = sys::Path::expandEnvironmentVariables("foo$PATH-bar", false);
    TEST_ASSERT_EQ(foopath_bar, "foo" + path + "-bar");

    auto foopath_bar_ = sys::Path::expandEnvironmentVariables("foo$PATH(bar)", false);
    TEST_ASSERT_FALSE(foopath_bar_.empty());
    TEST_ASSERT_EQ(foopath_bar_, "foo" + path + "(bar)");

    foopath_bar_ = sys::Path::expandEnvironmentVariables("foo$PATH)bar(", false);
    TEST_ASSERT_EQ(foopath_bar_, "foo" + path + ")bar(");

    foopath_bar_ = sys::Path::expandEnvironmentVariables("foo$(PATH)BAR)", false);
    TEST_ASSERT_EQ(foopath_bar_, "foo" + path + "BAR)");


    auto pathpath = sys::Path::expandEnvironmentVariables("$PATH$PATH", false);
    TEST_ASSERT_EQ(pathpath, path + path);
    pathpath = sys::Path::expandEnvironmentVariables("$PATH$(PATH)", false);
    TEST_ASSERT_EQ(pathpath, path + path);
    pathpath = sys::Path::expandEnvironmentVariables("${PATH}$PATH", false);
    TEST_ASSERT_EQ(pathpath, path + path);
    #if _WIN32  // %FOO% only on Windows
    pathpath = sys::Path::expandEnvironmentVariables("%PATH%%PATH%", false);
    TEST_ASSERT_EQ(pathpath, path + path);
    #endif
    auto pathpathpath = sys::Path::expandEnvironmentVariables("$PATH$(PATH)${PATH}", false);
    TEST_ASSERT_EQ(pathpathpath, pathpath + path);

    const auto foopath_barpathbar_ = sys::Path::expandEnvironmentVariables("foo$PATH-bar$(PATH)BAR)", false);
    TEST_ASSERT_EQ(foopath_barpathbar_, "foo" + path + "-bar" + path + "BAR)");
}

}

int main(int, char**)
{
    TEST_CHECK(testExpandEnvTilde);
    TEST_CHECK(testExpandEnvPath);

    return 0;
}

