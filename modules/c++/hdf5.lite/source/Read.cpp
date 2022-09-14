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

#include "except/Exception.h"
#include "hdf5/lite/HDF5Exception.h"

// see https://docs.hdfgroup.org/archive/support/HDF5/doc1.8/cpplus_RM/readdata_8cpp-example.html
#include <H5Cpp.h>

void hdf5::lite::fileRead(const coda_oss::filesystem::path& fileName, const std::string& datasetName)
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
        if (type_class == H5T_FLOAT)
        {
            const auto floattype = dataset.getFloatType();
            const size_t size = floattype.getSize();
            std::ignore = size;
        }

        /*
         * Get dataspace of the dataset.
         */
        const auto dataspace = dataset.getSpace();

        /*
         * Get the number of dimensions in the dataspace.
         */
        const auto rank = dataspace.getSimpleExtentNdims();
        std::ignore = rank;

      /*
         * Get the dimension size of each dimension in the dataspace.
         */
        hsize_t dims_out[2];
        const auto ndims = dataspace.getSimpleExtentDims(dims_out, NULL);
        std::ignore = ndims;

       #if 0 // TODO

        const int NX_SUB = 3;  // hyperslab dimensions
        const int NY_SUB = 4;
        const int NX = 7;  // output buffer dimensions
        const int NY = 7;
        const int NZ = 3;
        const int RANK_OUT = 3;

        /*
         * Define hyperslab in the dataset; implicitly giving strike and block NULL.
         */
        hsize_t offset[2];  // hyperslab offset in the file
        hsize_t count[2];  // size of the hyperslab in the file
        offset[0] = 1;
        offset[1] = 2;
        count[0] = NX_SUB;
        count[1] = NY_SUB;
        dataspace.selectHyperslab(H5S_SELECT_SET, count, offset);

        /*
         * Define the memory dataspace.
         */
        hsize_t dimsm[3]; /* memory space dimensions */
        dimsm[0] = NX;
        dimsm[1] = NY;
        dimsm[2] = NZ;
        H5::DataSpace memspace(RANK_OUT, dimsm);

        /*
         * Define memory hyperslab.
         */
        hsize_t offset_out[3];  // hyperslab offset in memory
        hsize_t count_out[3];  // size of the hyperslab in memory
        offset_out[0] = 3;
        offset_out[1] = 0;
        offset_out[2] = 0;
        count_out[0] = NX_SUB;
        count_out[1] = NY_SUB;
        count_out[2] = 1;
        memspace.selectHyperslab(H5S_SELECT_SET, count_out, offset_out);

        /*
         * Read data from hyperslab in the file into the hyperslab in
         * memory and display the data.
         */
        int data_out[NX][NY][NZ]; /* output buffer */
        dataset.read(data_out, H5::PredType::NATIVE_FLOAT, memspace, dataspace);
        #endif
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
