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

/*!
 * \file  DOMDocument.h
 * \brief File describing the tree API
 *
 * This is a simple alternative to the DOM API.  It is very similar,
 * but is a solution intended more for speed and simplicity.  The
 * DOM API offers far more functionality, where this version implements
 * only the widely used features.  In this file, we are only concerned
 * with the data structure for implementing the XML reader, not the reader
 * itself.
 */

#include <memory>
#include <coda_oss/string.h>

#include <config/Exports.h>

#include "DOMNode.h"
#include "Element.h"

namespace xml
{
namespace lite
{

/*!
 * \class DOMElement
 * \brief Wrapper around Element that tries to follow
 * https://xerces.apache.org/xerces-c/ApacheDOMC++Binding.html
 */
struct CODA_OSS_API DOMElement final : public DOMNode
{
    DOMElement(Element&);
    ~DOMElement();

    DOMElement(const DOMElement&) = delete;
    DOMElement& operator=(const DOMElement&) = delete;
    DOMElement(DOMElement&&) = default;
    DOMElement& operator=(DOMElement&&) = default;

    /*!
     *  See DOMElement.hpp
     */
    //coda_oss::u8string getNodeName() const override;
    coda_oss::u8string getNodeValue() const override;

    void setNodeValue(const coda_oss::u8string&) override;

    std::vector<xml::lite::DOMNode> getElementsByTagName(const std::string& tag) const;

private:
    Element* pElement;
};

}
}


