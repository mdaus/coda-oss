/* =========================================================================
 * This file is part of sys-c++ 
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
#ifndef CODA_OSS_sys_OS_h_INCLUDED_
#define CODA_OSS_sys_OS_h_INCLUDED_

#include "sys/AbstractOS.h"
#include "sys/Conf.h"

#ifdef _WIN32
#  include "sys/OSWin32.h"
namespace sys
{
typedef OSWin32 OS;
typedef DirectoryWin32 Directory;
}
#else
#  include "sys/OSUnix.h"
namespace sys
{
typedef OSUnix OS;
typedef DirectoryUnix Directory;
}
#endif

// This can be useful for code that will compile on all platforms, but needs different
// platform-specific behavior.  This avoids the use of more #ifdefs (no preprocessor)
// and also squelches compiler-warnings about unused local functions.
namespace sys
{
enum class PlatformType
{
    Windows,
    Linux,
    // MacOS
};

#if defined(_WIN32)
constexpr auto Platform = PlatformType::Windows;
#elif defined(CODA_OSS_POSIX2008_SOURCE)
constexpr auto Platform = PlatformType::Linux;
#else
#error "Unknown platform."
#endif

template <PlatformType>
inline std::string platformName();
template <>
inline std::string platformName<PlatformType::Windows>()
{
    return "Windows";
}
template <>
inline std::string platformName<PlatformType::Linux>()
{
    return "linux-gnu";
}

}

#endif  // CODA_OSS_sys_OS_h_INCLUDED_