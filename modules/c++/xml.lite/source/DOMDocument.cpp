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

#include "xml/lite/DOMDocument.h"

#include "xml/lite/Document.h"
#include "xml/lite/DOMElement.h"

xml::lite::DOMDocument::DOMDocument(std::shared_ptr<DOMConfiguration> config) :
    pConfiguration(config),
    pParser(std::make_unique<xml::lite::MinidomParser>()), pDocument(pParser->getDocument())
{
}
xml::lite::DOMDocument::DOMDocument(std::shared_ptr<DOMConfiguration> config, Document& doc) :
    pConfiguration(config), pDocument(&doc)
{
}

// See the table at https://xerces.apache.org/xerces-c/apiDocs-3/classDOMNode.html#aa92ed11988e02ff704493d97b18c2b37
std::string xml::lite::DOMDocument::getNodeName() const
{
    return "#document";
}

coda_oss::u8string xml::lite::DOMDocument::getNodeValue() const
{
    return coda_oss::u8string{}; // i.e., NULL
}

xml::lite::Uri xml::lite::DOMDocument::getNamespaceURI() const
{
    xml::lite::Uri retval;
    getRootElement(pDocument).getUri(retval);
    return retval;
}

xml::lite::DOMConfiguration& xml::lite::DOMDocument::getDomConfig()
{
    return *pConfiguration;
}
const xml::lite::DOMConfiguration& xml::lite::DOMDocument::getDomConfig() const
{
    return *pConfiguration;
}

xml::lite::DOMElement xml::lite::DOMDocument::createElementNS(const QName& q) const
{
    std::unique_ptr<Element> pElement(pDocument->createElement(q.getName(), q.getUri().value));
    return xml::lite::DOMElement(std::move(pElement));
}

const xml::lite::DOMElement xml::lite::DOMDocument::getDocumentElement() const
{
     return DOMElement(getRootElement(*pDocument));
 }
xml::lite::DOMElement xml::lite::DOMDocument::getDocumentElement()
{
    return DOMElement(getRootElement(*pDocument));
}
