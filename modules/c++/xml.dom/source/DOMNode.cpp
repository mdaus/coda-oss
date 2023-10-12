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

#include "xml/dom/DOMNode.h"

#include <stdexcept>

//coda_oss::u8string xml::lite::DOMNode::getNodeName() const
//{
//    throw std::logic_error("not implemented: DOMNode::getNodeName()");
//}

coda_oss::u8string xml::lite::DOMNode::getNodeValue() const
{
    throw std::logic_error("not implemented: DOMNode::getNodeValue()");
}

void xml::lite::DOMNode::setNodeValue(const coda_oss::u8string&)
{
    throw std::logic_error("not implemented: DOMNode::setNodeValue()");
}

xml::lite::Uri xml::lite::DOMNode::getNamespaceURI() const
{
    throw std::logic_error("not implemented: DOMNode::getNamespaceURI()");
}

std::string xml::lite::DOMNode::getPrefix() const
{
    throw std::logic_error("not implemented: DOMNode::getPrefix()");
}

std::string xml::lite::DOMNode::getLocalName() const
{
    throw std::logic_error("not implemented: DOMNode::getLocalName()");
}

void xml::lite::DOMNode::setPrefix(const std::string&) const
{
    throw std::logic_error("not implemented: DOMNode::setPrefix()");
}

