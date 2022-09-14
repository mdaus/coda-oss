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

#include "hdf5/lite/Read.h"

#include <stdexcept>
#include <tuple> // std::ignore
#include <vector>

#include "except/Exception.h"
#include "coda_oss/cstddef.h" // byte

#include "hdf5/lite/HDF5Exception.h"

// see https://docs.hdfgroup.org/archive/support/HDF5/doc1.8/cpplus_RM/readdata_8cpp-example.html
#include <H5Cpp.h>

static hsize_t getSimpleExtentSize(const H5::DataSet& dataset, std::vector<hsize_t>& dims_out)
{
    /*
     * Get dataspace of the dataset.
     */
    const auto dataspace = dataset.getSpace();

    /*
     * Get the number of dimensions in the dataspace.
     */
    const auto rank = dataspace.getSimpleExtentNdims();

    /*
     * Get the dimension size of each dimension in the dataspace.
     */
    dims_out.resize(rank);
    const auto ndims = dataspace.getSimpleExtentDims(dims_out.data(), nullptr);
    dims_out.resize(ndims);

    hsize_t retval = 1;
    for (const auto& dim : dims_out)
    {
        retval *= dim;
    }
    return retval;
}

static std::vector<hsize_t> fileRead_(const H5::DataSet& dataset, std::vector<double>& result)
{
    const auto type_class = dataset.getTypeClass();
    if (type_class != H5T_FLOAT)
    {
        throw std::invalid_argument("getTypeClass() should return H5T_FLOAT");
    }

    std::vector<hsize_t> dims_out;
    const auto count = getSimpleExtentSize(dataset, dims_out);

    result.resize(count);
    dataset.read(result.data(), H5::PredType::IEEE_F64LE);

    return dims_out;
}

std::vector<double> hdf5::lite::fileRead(const coda_oss::filesystem::path& fileName, const std::string& datasetName)
{
    try
    {
        /*
         * Turn off the auto-printing when failure occurs so that we can
         * handle the errors appropriately
         */
        H5::Exception::dontPrint();

        /*
         * Open the specified file and the specified dataset in the file.
         */
        H5::H5File file(fileName.string(), H5F_ACC_RDONLY);
        const auto dataset = file.openDataSet(datasetName);

        std::vector<double> retval;
        const auto dims_out = fileRead_(dataset, retval);
        return retval;
    }
    // catch failure caused by the H5File operations
    catch (const H5::FileIException& error)
    {
        const except::Context ctx(error.getDetailMsg(), __FILE__, __LINE__, error.getFuncName());
        throw except::IOException(ctx);
    }
    // catch failure caused by the DataSet operations
    catch (const H5::DataSetIException& error)
    {
        const except::Context ctx(error.getDetailMsg(), __FILE__, __LINE__, error.getFuncName());
        throw DataSetException11(ctx);
    }
    // catch failure caused by the DataSpace operations
    catch (const H5::DataSpaceIException& error)
    {
        const except::Context ctx(error.getDetailMsg(), __FILE__, __LINE__, error.getFuncName());
        throw DataSpaceException11(ctx);
    }
    // catch failure caused by the DataType operations
    catch (const H5::DataTypeIException& error)
    {
        const except::Context ctx(error.getDetailMsg(), __FILE__, __LINE__, error.getFuncName());
        throw DataTypeException11(ctx);
    }
}
