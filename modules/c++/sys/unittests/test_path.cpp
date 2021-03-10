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
TEST_CASE(testPathMerge)
{
    const sys::OS os;

    // PATH is usually set to multiple directories on both Windows and *nix
    std::vector<std::string> paths;
    bool result = os.splitEnv("PATH", paths);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_GREATER(paths.size(), 0);

    const auto components = sys::Path::separate(paths[0]);
    TEST_ASSERT_GREATER(components.size(), 0);
    const auto path = sys::Path::merge(components);
    TEST_ASSERT_FALSE(path.empty());
    TEST_ASSERT_TRUE(sys::Filesystem::exists(path));
    TEST_ASSERT_TRUE(sys::Filesystem::is_directory(path));
}

TEST_CASE(testExpandEnvTilde)
{
    const auto result = sys::Path::expandEnvironmentVariables("~");
    TEST_ASSERT_FALSE(result.empty());
}

TEST_CASE(testExpandEnvPath)
{
    const auto path = sys::Path::expandEnvironmentVariables("$PATH", false);
    TEST_ASSERT_FALSE(path.empty());

    const auto path2 = sys::Path::expandEnvironmentVariables("$(PATH)", false);
    TEST_ASSERT_FALSE(path2.empty());
    TEST_ASSERT_EQ(path2, path);

    const auto path3 = sys::Path::expandEnvironmentVariables("${PATH}", false);
    TEST_ASSERT_FALSE(path3.empty());
    TEST_ASSERT_EQ(path3, path);

    const auto foopath = sys::Path::expandEnvironmentVariables("foo${PATH}", false);
    TEST_ASSERT_FALSE(foopath.empty());
    TEST_ASSERT_EQ(foopath, "foo" + path);

    const auto pathfoo = sys::Path::expandEnvironmentVariables("${PATH}foo", false);
    TEST_ASSERT_FALSE(pathfoo.empty());
    TEST_ASSERT_EQ(pathfoo, path + "foo");

    const auto foopathbar = sys::Path::expandEnvironmentVariables("foo${PATH}bar", false);
    TEST_ASSERT_FALSE(foopathbar.empty());
    TEST_ASSERT_EQ(foopathbar, "foo" + path + "bar");

    const auto foopath_bar = sys::Path::expandEnvironmentVariables("foo$PATH-bar", false);
    TEST_ASSERT_FALSE(foopath_bar.empty());
    TEST_ASSERT_EQ(foopath_bar, "foo" + path + "-bar");

    auto foopath_bar_ = sys::Path::expandEnvironmentVariables("foo$PATH(bar)", false);
    TEST_ASSERT_FALSE(foopath_bar_.empty());
    TEST_ASSERT_EQ(foopath_bar_, "foo" + path + "(bar)");

    foopath_bar_ = sys::Path::expandEnvironmentVariables("foo$PATH)bar(", false);
    TEST_ASSERT_FALSE(foopath_bar_.empty());
    TEST_ASSERT_EQ(foopath_bar_, "foo" + path + ")bar(");

    foopath_bar_ = sys::Path::expandEnvironmentVariables("foo$(PATH)BAR)", false);
    TEST_ASSERT_FALSE(foopath_bar_.empty());
    TEST_ASSERT_EQ(foopath_bar_, "foo" + path + "BAR)");
}

}

int main(int, char**)
{
    TEST_CHECK(testPathMerge);
    TEST_CHECK(testExpandEnvTilde);
    TEST_CHECK(testExpandEnvPath);

    return 0;
}

