/* =========================================================================
 * This file is part of sys-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * (C) Copyright 2023, Maxar Technologies, Inc.
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

#include "sys/File.h"

#include "sys/Path.h"

sys::File sys::make_File(const coda_oss::filesystem::path& path, int accessFlags, int creationFlags)
{
    return sys::File(path.string(), accessFlags, creationFlags);
}

sys::File sys::make_File(const coda_oss::filesystem::path& parent, const coda_oss::filesystem::path& name,
        int accessFlags, int creationFlags)
{
    return sys::File(parent.string(), name.string(), accessFlags, creationFlags);
}