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

#include <functional>
#include <stdexcept>
#include <utility>

// see https://docs.hdfgroup.org/archive/support/HDF5/doc1.8/cpplus_RM/readdata_8cpp-example.html
#include <H5Cpp.h>

#include "except/Exception.h"
#include "hdf5/lite/HDF5Exception.h"

// Utility routines for INTERNAL use!

namespace hdf5
{
namespace lite
{
namespace details
{
// Call the given function, converting H5 exceptions to our own.
void try_catch_H5Exceptions_(std::function<void(void*)> f, void* context = nullptr);

template<typename TReturn, typename TFunc, typename ...TArgs>
auto try_catch_H5Exceptions(TFunc f, TArgs&&... args)
{
    // Figure out return type by "calling" the function at compile-time
    //decltype(f(std::forward<TArgs>(args)...)) retval;
    TReturn retval; 
    
    // "Hide" the arguments inside of a lambda
    auto call_f = [&](void*) { retval = f(std::forward<TArgs>(args)...); };
    details::try_catch_H5Exceptions_(call_f);
    return retval;
}

}
}
}
