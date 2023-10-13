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

#include "xml/lite/DOMAttr.h"

#include "str/Convert.h"

#include "xml/lite/Attributes.h"

xml::lite::DOMAttr::DOMAttr(AttributeNode& node) :
    pAttributeNode(&node)
{
}

xml::lite::Uri xml::lite::DOMAttr::getNamespaceURI() const
{
    xml::lite::Uri retval;
    pAttributeNode->getUri(retval);
    return retval;
}

std::string xml::lite::DOMAttr::getPrefix() const
{
    return pAttributeNode->getPrefix();
}

std::string xml::lite::DOMAttr::getLocalName() const
{
    return pAttributeNode->getLocalName();
}
std::string xml::lite::DOMAttr::getName() const
{
    return getLocalName();
}

std::string xml::lite::DOMAttr::getValue() const
{
    return pAttributeNode->getValue();
}
void xml::lite::DOMAttr::setValue(const std::string& v)
{
    pAttributeNode->setValue(v);
}