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

#ifndef CODA_OSS_hdf5_lite_Read_h_INCLUDED_
#define CODA_OSS_hdf5_lite_Read_h_INCLUDED_
#pragma once

/*!
 * \file  Read.h
 * \brief HDF File-reading API
 *
 * These are simple routines to read HDF5 files; they're modeled after the MATLab API
 * https://www.mathworks.com/help/matlab/import_export/import-hdf5-files.html
 */

#include <string>

#include "config/Exports.h"
#include "sys/filesystem.h"

namespace hdf5
{
namespace lite
{

CODA_OSS_API void fileRead(const coda_oss::filesystem::path&, const std::string& datasetName);
//CODA_OSS_API void locationRead(const std::string& loc, const std::string& datasetName); // e.g, s3://

}
}

#endif // CODA_OSS_hdf5_lite_Read_h_INCLUDED_
