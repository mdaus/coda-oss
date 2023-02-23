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

#ifndef CODA_OSS_hdf5_lite_highfive_h_INCLUDED_
#define CODA_OSS_hdf5_lite_highfive_h_INCLUDED_
#pragma once

/*!
 * \file  Read.h
 * \brief HDF File-reading API
 *
 * These are simple routines to read HDF5 files; they're loosely modeled after the MATLab API
 * https://www.mathworks.com/help/matlab/import_export/import-hdf5-files.html
 */

#include <string>

#include "highfive/H5Easy.hpp"
#include "highfive/H5DataSet.hpp"

#include "SpanRC.h"

namespace hdf5
{
namespace lite
{
    template<typename T>
    inline HighFive::DataSet writeDataSet(H5Easy::File& file,
        SpanRC<T> data, const std::string& dataset_name /*TODO ...*/)
    {
        const std::vector<size_t> dims{data.dims().row, data.dims().col};
        const HighFive::DataSpace dataspace{dims};
        auto retval = file.createDataSet<T>(dataset_name, dataspace);
        retval.write_raw(data.data());
        return retval;
    }
}
}

#endif // CODA_OSS_hdf5_lite_highfive_h_INCLUDED_
