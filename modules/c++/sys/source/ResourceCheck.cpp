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

#include <sys/ResourceCheck.h>
#include <algorithm>

namespace sys
{

bool canProcessFit(size_t jobMemBytes, 
                   size_t systemMemBytes,
                   size_t reservedBytes,
                   double margin)
{
    return jobMemBytes < getAvailableMem(systemMemBytes,
                                         reservedBytes,
                                         margin);
}

size_t getAvailableMem(size_t systemMemBytes,
                       size_t reservedBytes,
                       double margin)
{
    size_t relativeAvailable = systemMemBytes * margin;
    if (reservedBytes > systemMemBytes)
    {
        return 0;
    }

    size_t absAvailable = systemMemBytes - reservedBytes;
    return std::min<size_t>(relativeAvailable, absAvailable);
}

}
