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

static hdf5::lite::FileInfo fileInfo_(const coda_oss::filesystem::path& fileName)
{
    hdf5::lite::FileInfo retval;
    retval.name = fileName.string();

    /*
     * Open the specified file and the specified dataset in the file.
     */
    H5::H5File file(retval.name, H5F_ACC_RDONLY);

    return retval;
}

hdf5::lite::FileInfo hdf5::lite::fileInfo(coda_oss::filesystem::path fileName)
{
    return details::try_catch_H5Exceptions(fileInfo_, fileName);
}
