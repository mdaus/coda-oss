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
xml::lite::DOMElement xml::lite::DOMDocument::createElementNS(const Uri& uri, const std::string& q) const
{
    return createElementNS(QName(uri, q));
}

 xml::lite::DOMElement xml::lite::DOMDocument::getDocumentElement() const
{
     return DOMElement(getRootElement(*pDocument));
 }
