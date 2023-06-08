/* =========================================================================
 * This file is part of std-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * (C) Copyright 2023, Maxar Technologies, Inc.
 *
 * std-c++ is free software; you can redistribute it and/or modify
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


#ifndef CODA_OSS_import_cpp14_h_INCLUDED_
#define CODA_OSS_import_cpp14_h_INCLUDED_
#pragma once

#include "coda_oss/CPlusPlus.h"
#include "config/disable_compiler_warnings.h"

// Common C++14 headers
#include <limits>
#include <memory>
#include <new>

CODA_OSS_disable_warning_push
#ifdef _MSC_VER
#pragma warning(disable: 4996) // '...': warning STL4037: The effect of instantiating the template std::complex for any type other than float, double, or long double is unspecified. You can define _SILENCE_NONFLOATING_COMPLEX_DEPRECATION_WARNING to suppress this warning.
#endif

#include <complex>
namespace
{
    std::complex<short> unused;
}
CODA_OSS_disable_warning_pop

#include <utility>
#include <tuple>
#include <future>
#include <typeinfo>
#include <type_traits>
#include <stdexcept>
#include <locale>

#include <iomanip>
#include <iostream>
#include <ostream>
#include <sstream>

#include <array>
#include <string>
#include <vector>
#include <map>
#include <set>

#include <numeric>
#include <functional>
#include <algorithm>
#include <iterator>

#endif // CODA_OSS_import_cpp14_h_INCLUDED_


