/* =========================================================================
 * This file is part of xml.lite-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * © Copyright 2023, Maxar Technologies, Inc.
 *
 * xml.lite-c++ is free software; you can redistribute it and/or modify
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

#include <string>
#include <vector>
#include <coda_oss/optional.h>
#include <map>

#include <config/Exports.h>


/*!
 * \file DOMConfiguration.h
 * https://xerces.apache.org/xerces-c/ApacheDOMC++Binding.html
 *
 */

namespace xml
{
namespace lite
{

using DOMStringList = std::vector<std::string>;

/*!
 * \class DOMConfiguration
 * \brief Simple wrapper around MinidomParser.
 *
 */
class CODA_OSS_API DOMConfiguration final
{
    std::map<std::string, bool> parameters;

public:

    /*!
     *  See DOMConfiguration.hpp
     */
    void setParameter(const std::string&, bool);
    coda_oss::optional<bool> getParameter(const std::string&) const;
    bool canSetParameter(const std::string&, bool) const;
    DOMStringList getParameterNames() const;
};

}
}

