/* =========================================================================
 * This file is part of config-c++
 * =========================================================================
 *
 * (C) Copyright 2021, 2022, Maxar Technologies, Inc.
 *
 * config-c++ is free software; you can redistribute it and/or modify
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

#pragma once
#ifndef CODA_OSS_config_CPlusPlus_h_INCLUDED_
#define CODA_OSS_config_CPlusPlus_h_INCLUDED_

#ifdef CODA_OSS_cplusplus 
    #error "CODA_OSS_cplusplus already #define'd."
#endif

#ifndef __cplusplus
    #error "Only C++ compilation is supported."
#endif
#define CODA_OSS_cplusplus __cplusplus

// https://en.cppreference.com/w/cpp/preprocessor/replace#Predefined_macros
#define CODA_OSS_cplusplus23 202302L

// Define a few macros as that's less verbose than testing against a version number
#define CODA_OSS_cpp23 (CODA_OSS_cplusplus >= CODA_OSS_cplusplus23)

#if !CODA_OSS_cpp23
    #error "Must compile with C++23 or greater."
#endif

#endif // CODA_OSS_config_CPlusPlus_h_INCLUDED_
