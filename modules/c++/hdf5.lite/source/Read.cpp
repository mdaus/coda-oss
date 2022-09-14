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

        /*
         * Get the class of the datatype that is used by the dataset.
         */
        const auto type_class = dataset.getTypeClass();
        size_t size = 0;
        if (type_class == H5T_FLOAT)
        {
            const auto floattype = dataset.getFloatType();
            size = floattype.getSize();
        }

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
        std::vector<hsize_t> dims_out(rank);
        const auto ndims = dataspace.getSimpleExtentDims(dims_out.data(), nullptr);
        dims_out.resize(ndims);

        /*
         * Read data from hyperslab in the file into the hyperslab in
         * memory and display the data.
         */
        size_t size_in_bytes = 1;
        for (const auto& dim : dims_out)
        {
            size_in_bytes *= dim;
        }
        size_in_bytes *= size;

        //std::vector<double> data_out(dims_out[0]); /* output buffer */
        std::vector<coda_oss::byte> data_out(size_in_bytes); /* output buffer */
        dataset.read(data_out.data(), H5::PredType::IEEE_F64LE);

        const void* pData = data_out.data();
        auto pRetval = static_cast<const double*>(pData);
        return std::vector<double>(pRetval, pRetval + (data_out.size() / size));
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
