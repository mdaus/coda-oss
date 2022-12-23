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
#include <functional>

#include "except/Exception.h"
#include "coda_oss/cstddef.h" // byte

#include "hdf5/lite/HDF5Exception.h"

#include "H5.h"

static void try_catch_H5Exception(std::function<void(void*)> f, void* context=nullptr)
{
    try
    {
        /*
         * Turn off the auto-printing when failure occurs so that we can
         * handle the errors appropriately
         */
        H5::Exception::dontPrint();

        f(context);
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
        throw hdf5::lite::DataSetException11(ctx);
    }
    // catch failure caused by the DataSpace operations
    catch (const H5::DataSpaceIException& error)
    {
        const except::Context ctx(error.getDetailMsg(), __FILE__, __LINE__, error.getFuncName());
        throw hdf5::lite::DataSpaceException11(ctx);
    }
    // catch failure caused by the DataType operations
    catch (const H5::DataTypeIException& error)
    {
        const except::Context ctx(error.getDetailMsg(), __FILE__, __LINE__, error.getFuncName());
        throw hdf5::lite::DataTypeException11(ctx);
    }
}

template<typename T>
static types::RowCol<size_t> readDatasetT(const H5::DataSet& dataset, H5T_class_t type_class, const H5::DataType& mem_type,
    std::vector<T>& result)
{
    if (type_class != dataset.getTypeClass())
    {
        throw std::invalid_argument("getTypeClass() returned wrong value.");
    }

    const auto retval = hdf5::lite::details::getSimpleExtentSize(dataset);
    result.resize(retval.area());
    dataset.read(result.data(), mem_type);

    return retval;
}

inline types::RowCol<size_t> readDataset_(const H5::DataSet& dataset, std::vector<float>& result)
{
    static_assert(sizeof(float) * 8 == 32, "'float' should be 32-bits"); // IEEE_F32LE
    return readDatasetT(dataset, H5T_FLOAT, H5::PredType::IEEE_F32LE, result);
}

inline types::RowCol<size_t> readDataset_(const H5::DataSet& dataset, std::vector<double>& result)
{
    static_assert(sizeof(double) * 8 == 64, "'double' should be 64-bits"); // IEEE_F64LE
    return readDatasetT(dataset, H5T_FLOAT, H5::PredType::IEEE_F64LE, result);
}

static types::RowCol<size_t> readFile_(const coda_oss::filesystem::path& fileName, const std::string& datasetName,
                                std::vector<double>& result)
{
    /*
     * Open the specified file and the specified dataset in the file.
     */
    H5::H5File file(fileName.string(), H5F_ACC_RDONLY);
    const auto dataset = file.openDataSet(datasetName);
    return readDataset_(dataset, result);
}
types::RowCol<size_t> hdf5::lite::readFile(coda_oss::filesystem::path fileName, std::string datasetName,
    std::vector<double>& result)
{
    types::RowCol<size_t> retval;
    auto call_readFile_ = [&](void*) { retval = readFile_(fileName, datasetName, result); };
    try_catch_H5Exception(call_readFile_);
    return retval;
}
