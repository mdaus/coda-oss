/* =========================================================================
 * This file is part of hdf5.lite-c++
 * =========================================================================
 *
 * (C) Copyright 2022, Maxar Technologies, Inc.
 *
 * hdf5.lite-c++ is free software; you can redistribute it and/or modify
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

#include "import/std.h"
#include <std/filesystem>

#include <TestCase.h>

#include "sys/FileFinder.h"

#include "highfive/H5Easy.hpp"
#include "highfive/H5DataSet.hpp"
#include "highfive/H5File.hpp"
#include "hdf5/lite/Read.h"
#include "hdf5/lite/HDF5Exception.h"

static std::filesystem::path find_unittest_file(const std::filesystem::path& name)
{
    static const auto unittests = std::filesystem::path("modules") / "c++" / "hdf5.lite" / "unittests";
    return sys::test::findGITModuleFile("coda-oss", unittests, name);
}

TEST_CASE(test_highfive_load)
{
    static const auto path = find_unittest_file("example.h5");

    const H5Easy::File file(path.string());
    const auto lat = H5Easy::load<std::vector<double>>(file, "/g4/lat");
    TEST_ASSERT_EQ(lat.size(), 19);
    TEST_ASSERT_ALMOST_EQ(lat[0], -90.0);
    TEST_ASSERT_ALMOST_EQ(lat[0], -lat[18]);
}

TEST_CASE(test_highfive_FileException)
{
    static const std::filesystem::path path = "does not exist . h5";
    TEST_SPECIFIC_EXCEPTION(H5Easy::File(path.string()), HighFive::FileException);
}

TEST_CASE(test_highfive_nested)
{
    /*
    Group '/' 
    Group '/1' 
        Group '/1/bar' 
            Group '/1/bar/cat' 
                Dataset 'i' 
                    Size:  10x1
                    MaxSize:  10x1
                    Datatype:   H5T_IEEE_F64LE (double)
                    ChunkSize:  []
                    Filters:  none
                    FillValue:  0.000000
    */
    static const auto path = find_unittest_file("123_barfoo_catdog_cx.h5");

    const H5Easy::File file(path.string());
    const auto i = H5Easy::load<std::vector<std::vector<double>>>(file, "/1/bar/cat/i");
    TEST_ASSERT_EQ(i.size(), 1);
    TEST_ASSERT_EQ(i[0].size(), 10);

    const auto r = H5Easy::load<std::vector<std::vector<double>>>(file, "/1/bar/dog/r");
    TEST_ASSERT_EQ(r.size(), 1);
    TEST_ASSERT_EQ(r[0].size(), 10);
    
    TEST_ASSERT_EQ(i.size(), r.size());
    TEST_ASSERT_EQ(i[0].size(), r[0].size());
}

TEST_CASE(test_highfive_nested_small)
{
    // top group: Data
    // outer groups: 1, 2, 3, 4, 5
    // sub groups: bar, foo
    // sub-sub groups: cat, dog
    // sub-sub-sub groups: a, b, c, d
    // data: i (float array), r (float array)
    static const auto path = find_unittest_file("nested_complex_float32_data_small.h5");

    const H5Easy::File file(path.string());
    const auto i = H5Easy::load<std::vector<double>>(file, "/Data/1/bar/cat/a/i");
    TEST_ASSERT_EQ(i.size(), 10);
    auto actual = std::accumulate(i.cbegin(), i.cend(), 0.0);
    TEST_ASSERT_EQ(actual, 0.0);

    const auto r = H5Easy::load<std::vector<double>>(file, "/Data/5/foo/dog/d/r");
    TEST_ASSERT_EQ(r.size(), 10);
    actual = std::accumulate(r.cbegin(), r.cend(), 0.0);
    TEST_ASSERT_EQ(actual, 10.0);
}

TEST_CASE(test_highfive_nested_small_wrongType)
{
    static const auto path = find_unittest_file("nested_complex_float32_data_small.h5");

    const H5Easy::File file(path.string());
    TEST_SPECIFIC_EXCEPTION(
        H5Easy::load<std::vector<std::complex<float>>>(file, "/Data/1/bar/cat/a/i"),
        HighFive::DataSetException);
}

TEST_MAIN(
    TEST_CHECK(test_highfive_load);
    TEST_CHECK(test_highfive_FileException);
    TEST_CHECK(test_highfive_nested);
    TEST_CHECK(test_highfive_nested_small);
    TEST_CHECK(test_highfive_nested_small_wrongType);
)
