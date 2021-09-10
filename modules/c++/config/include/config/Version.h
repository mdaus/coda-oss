/* =========================================================================
 * This file is part of config-c++
 * =========================================================================
 *
 * (C) Copyright 2021, Maxar Technologies, Inc.
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
#ifndef CODA_OSS_config_Version_h_INCLUDED_
#define CODA_OSS_config_Version_h_INCLUDED_
#pragma once

// Do this ala C++ ... we don't currently have major/minor/patch

#define CODA_OSS_VERSION 20210910L
namespace config
{
constexpr long coda_oss_version = CODA_OSS_VERSION;
}

#endif  // CODA_OSS_config_Version_h_INCLUDED_