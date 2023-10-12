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

#include "xml/lite/DOMConfiguration.h"


#include <assert.h>

static const std::map<std::string, bool> defaultParameters
{
        {"prettyPrint", false},  // Element::prettyPrint()
        {"consoleOutput", false},  // Element::consoleOutput_
};

xml::lite::DOMConfiguration::DOMConfiguration() : parameters(defaultParameters)
{
}

void xml::lite::DOMConfiguration::setParameter(const std::string& name, bool value)
{
    parameters[name] = value;
}

coda_oss::optional<bool> xml::lite::DOMConfiguration::getParameter(const std::string& name) const
{
    coda_oss::optional<bool> retval;
    auto it = parameters.find(name);
    if (it != parameters.end())
    {
        retval = it->second;
    }
    return retval;
}

bool xml::lite::DOMConfiguration::canSetParameter(const std::string&, bool) const
{
    return true; // can set any parameter to any value
}

xml::lite::DOMStringList xml::lite::DOMConfiguration::getParameterNames() const
{
    xml::lite::DOMStringList retval;
    retval.reserve(parameters.size());
    for (auto&& kv : parameters)
    {
        retval.push_back(kv.first);
    }
    return retval;
}

