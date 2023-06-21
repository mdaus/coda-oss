/* =========================================================================
 * This file is part of coda-oss
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * sys-c++ is free software; you can redistribute it and/or modify
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
#ifndef CODA_OSS_import_vectorclass_h_INCLUDED_
#define CODA_OSS_import_vectorclass_h_INCLUDED_

#include "coda_oss/CPlusPlus.h"
#include "config/compiler_extensions.h"

#ifndef VCL_NAMESPACE
#define VCL_NAMESPACE vcl // put stuff in vcl namespace
#endif

CODA_OSS_disable_warning_system_header_push
#if !defined(_MSC_VER)
CODA_OSS_disable_warning(-Wunused-but-set-variable)
CODA_OSS_disable_warning(-Wunused-but-set-parameter)
CODA_OSS_disable_warning(-Wzero-as-null-pointer-constant)
#endif
#if CODA_OSS_cpp17
#include "vectorclass/version2/vectorclass.h"
#include "vectorclass/version2/vectormath_trig.h"
#else
#include "vectorclass/version1/vectorclass.h"
#include "vectorclass/version2/vectormath_trig.h"
#endif
CODA_OSS_disable_warning_pop

#endif // CODA_OSS_import_vectorclass_h_INCLUDED_

