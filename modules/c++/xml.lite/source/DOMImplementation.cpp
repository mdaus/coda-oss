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

#include "xml/lite/DOMImplementation.h"

xml::lite::DOMImplementation::DOMImplementation() :
    pConfiguration(std::make_shared<DOMConfiguration>())
{
}
xml::lite::DOMImplementation::~DOMImplementation() = default;

xml::lite::DOMDocument xml::lite::DOMImplementation::createDocument() const
{
    return xml::lite::DOMDocument(pConfiguration);
}
//DOMDocument createDocument(const QName&) const;
//DOMDocument createDocument(const Uri&, const std::string&) const;

xml::lite::DOMParser xml::lite::DOMImplementation::createParser() const
{
    return xml::lite::DOMParser(pConfiguration);
}

xml::lite::DOMSerializer xml::lite::DOMImplementation::createSerializer() const
{
    return xml::lite::DOMSerializer(pConfiguration);
}