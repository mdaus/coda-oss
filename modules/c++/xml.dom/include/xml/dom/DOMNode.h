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

#include <memory>
#include <coda_oss/string.h>
#include <vector>

#include <config/Exports.h>

#include <xml/lite/QName.h>

namespace xml
{
namespace lite
{
/*!
 * \class DOMNode
 * \brief Base class for DOMDocument, DOMAttribute, DOMElement, etc.
 * https://xerces.apache.org/xerces-c/ApacheDOMC++Binding.html
 */
struct CODA_OSS_API DOMNode
{
protected:
    DOMNode() = default;

public:
    virtual ~DOMNode() = default;

    DOMNode(const DOMNode&) = delete;
    DOMNode& operator=(const DOMNode&) = delete;
    DOMNode(DOMNode&&) = default;
    DOMNode& operator=(DOMNode&&) = default;

    /*!
     *  See DOMNode.hpp
     */
    //virtual coda_oss::u8string getNodeName() const;
    virtual coda_oss::u8string getNodeValue() const;

    virtual void setNodeValue(const coda_oss::u8string&);

    virtual Uri getNamespaceURI() const;
    virtual std::string getPrefix() const;
    virtual std::string getLocalName() const;
    virtual void setPrefix(const std::string&) const;

};

using DOMNodeList = std::vector<std::unique_ptr<xml::lite::DOMNode>>;

}
}


