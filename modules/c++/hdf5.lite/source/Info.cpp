/* =========================================================================
 * This file is part of hd5.lite-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2022, Maxar Technologies, Inc.
 *
 * hd5.lite-c++ is free software; you can redistribute it and/or modify
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

#include "hdf5/lite/Info.h"

#include <tuple> // std::ignore

#include "H5.h"
#include "hdf5.lite.h"


// https://docs.hdfgroup.org/archive/support/HDF5/doc1.8/cpplus_RM/h5group_8cpp-example.html
static herr_t group_info(hid_t loc_id, const char *name, const H5L_info_t* /*linfo*/, void *opdata)
{
    // only interested in groups
    const auto obj = H5Gopen2(loc_id, name, H5P_DEFAULT);
    if (obj > 0)
    {
        H5Gclose(obj);

        hdf5::lite::GroupInfo info;
        info.name = name;

        auto pRetval = static_cast<std::vector<hdf5::lite::GroupInfo>*>(opdata);
        pRetval->push_back(info);
    }

    return 0;
}
static std::vector<hdf5::lite::GroupInfo> getGroups(H5::Group& group)
{
    std::vector<hdf5::lite::GroupInfo> retval;

    const auto herr = H5Literate(group.getId(), H5_INDEX_NAME, H5_ITER_INC, nullptr /*idx*/, group_info,  &retval);
    if (herr != 0)
    {
        // How can this happen?
        throw std::logic_error("H5Literate failed.");
    }

    return retval;
 }

// https://docs.hdfgroup.org/archive/support/HDF5/doc1.8/cpplus_RM/h5group_8cpp-example.html
static herr_t dataset_info(hid_t loc_id, const char *name, const H5L_info_t* /*linfo*/, void *opdata)
{
    // only interested in Datasets
    const auto obj = H5Dopen2(loc_id, name, H5P_DEFAULT);
    if (obj > 0)
    {
        H5Dclose(obj);

        hdf5::lite::DatasetInfo info;
        info.name = name;

        auto pRetval = static_cast<std::vector<hdf5::lite::DatasetInfo>*>(opdata);
        pRetval->push_back(info);
    }

    return 0;
}
static std::vector<hdf5::lite::DatasetInfo> getDatasets(H5::Group& group)
 {
    std::vector<hdf5::lite::DatasetInfo> retval;

    const auto herr = H5Literate(group.getId(), H5_INDEX_NAME, H5_ITER_INC, nullptr /*idx*/, dataset_info,  &retval);
    if (herr != 0)
    {
        // How can this happen?
        throw std::logic_error("H5Literate failed.");
    }

    return retval;

 }

// https://docs.hdfgroup.org/archive/support/HDF5/doc1.8/cpplus_RM/readdata_8cpp-example.html
static hdf5::lite::FileInfo fileInfo_(coda_oss::filesystem::path filename, std::string loc)
{
    hdf5::lite::FileInfo retval;
    retval.filename = filename.string();
    retval.name = loc;

    /*
     * Open the specified file and the specified dataset in the file.
     */
    H5::H5File file(retval.filename, H5F_ACC_RDONLY);
    auto group = file.openGroup(retval.name);

    retval.groups = getGroups(group);
    retval.datasets = getDatasets(group);

    return retval;
}
hdf5::lite::FileInfo hdf5::lite::fileInfo(coda_oss::filesystem::path filename, std::string loc)
{
    return details::try_catch_H5Exceptions(fileInfo_, filename, loc);
}
hdf5::lite::FileInfo hdf5::lite::fileInfo(coda_oss::filesystem::path filename)
{
    return fileInfo(filename, "/" /*loc*/);
}

