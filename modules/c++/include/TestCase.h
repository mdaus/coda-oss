/* =========================================================================
 * This file is part of CODA-OSS
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * CODA-OSS is free software; you can redistribute it and/or modify
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

#ifndef CODA_OSS_TestCase_h_INCLUDED_
#define CODA_OSS_TestCase_h_INCLUDED_

#ifdef __cplusplus
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>
#include <std/filesystem>
#include <sys/OS.h>

// To ease adoption of better test framework, provide backwards compatibility macros.
#define TEST_ASSERT_ALMOST_EQ_EPS(X1, X2, EPS) REQUIRE_EQ(X1, doctest::Approx(X2).epsilon(EPS));
#define TEST_ASSERT_ALMOST_EQ(X1, X2) TEST_ASSERT_ALMOST_EQ_EPS(X1, X2,  std::numeric_limits<float>::epsilon());
#define TEST_FAIL FAIL("Should never get here");
#define TEST_FAIL_MSG(msg) FAIL(msg);

#define TEST_ASSERT(X) REQUIRE(X);
#define TEST_ASSERT_EQ(X, Y) REQUIRE_EQ(X, Y);
#define TEST_ASSERT_TRUE(X) REQUIRE(X);
#define TEST_ASSERT_FALSE(X) REQUIRE_FALSE(X);
#define TEST_THROWS(X) REQUIRE_THROWS(X);
#define TEST_EXCEPTION(X) REQUIRE_THROWS(X);
#define TEST_SPECIFIC_EXCEPTION(X, ex) REQUIRE_THROWS_AS(X, ex);
#define TEST_SUCCESS REQUIRE(true);
#define TEST_ASSERT_NOT_EQ(X, Y) REQUIRE_NE(X, Y);
#define TEST_ASSERT_GREATER(X, Y) REQUIRE_GT(X, Y);
#define TEST_ASSERT_GREATER_EQ(X, Y) REQUIRE_GE(X, Y);
#define TEST_ASSERT_LESSER(X, Y) REQUIRE_LT(X, Y);
#define TEST_ASSERT_LESSER_EQ(X, Y) REQUIRE_LE(X, Y);
#define TEST_ASSERT_NULL(X) REQUIRE(X == nullptr);
#define TEST_ASSERT_NOT_NULL(X) REQUIRE(X != nullptr);

namespace TestCase
{
// These variables are set by ctest based on logic in coda_add_tests and allow us to query relative paths which contain
// test data or files that we know exist
namespace fs = std::filesystem;
/*!
 * \brief Get root binary directory (see CMAKE_BINARY_DIR)
 */
fs::path get_top_binary_dir()
{
    return fs::path(sys::getEnv("TEST_CASE_BINARY_DIR"));
}

/*!
 * \brief Get binary directory corresponding to the test being run (see CMAKE_CURRENT_BINARY_DIR)
 */
fs::path get_this_binary_dir()
{
    return fs::path(sys::getEnv("TEST_CASE_THIS_BINARY_DIR"));
}

/*!
 * \brief Get root source directory (see CMAKE_SOURCE_DIR)
 */
fs::path get_top_source_dir()
{
    return fs::path(sys::getEnv("TEST_CASE_SOURCE_DIR"));
}

/*!
 * \brief Get source directory corresponding to the test being run (see CMAKE_CURRENT_SOURCE_DIR)
 */
fs::path get_this_source_dir()
{
    return fs::path(sys::getEnv("TEST_CASE_THIS_SOURCE_DIR"));
}
}
#endif
#endif  // CODA_OSS_TestCase_h_INCLUDED_
