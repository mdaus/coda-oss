/* =========================================================================
 * This file is part of xml.easy-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * xml.easy-c++ is free software; you can redistribute it and/or modify
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

#ifndef CODA_OSS_xml_easy_ElementReference_h_INCLUDED_
#define CODA_OSS_xml_easy_ElementReference_h_INCLUDED_
#pragma once

#include <coda_oss/string.h>

#include <memory>
#include <string>

#include "xml/easy/Attribute.h"
#include "xml/lite/Element.h"
#include "xml/lite/QName.h"

namespace xml
{
namespace easy
{

/*!
 * \class Element
 * \brief The class defining one element of an XML document
 *
 * This class stores all of the element information about an XML
 * document.
 */
struct Element;
struct ElementReference final
{
    ElementReference(xml::lite::Element&);
    ElementReference(Element&);

    ElementReference(const ElementReference&) = delete;
    ElementReference& operator=(const ElementReference&) = delete;
    ElementReference(ElementReference&&) = default;
    ElementReference& operator=(ElementReference&&) = default;

    ~ElementReference() = default;

    xml::lite::Element& ref();
    const xml::lite::Element& ref() const;
    const xml::lite::Element& cref();
    const xml::lite::Element& cref() const;

    ElementReference& operator=(const std::string&);  // setCharacterData()
    ElementReference& operator=(const char*);  // setCharacterData()
    ElementReference& operator=(const xml::lite::QName&);  // setQName()
    ElementReference& operator=(const xml::lite::Uri&);  // setUri()
    ElementReference& operator=(std::unique_ptr<xml::lite::Element>&&);  // setChild()
    ElementReference& operator=(Element&&);  // setChild()

private:
    xml::lite::Element& element_;
};

ElementReference addChild(ElementReference&, std::unique_ptr<xml::lite::Element>&&);
void operator+=(ElementReference&, std::unique_ptr<xml::lite::Element>&&);  // addChild()
ElementReference addChild(ElementReference&, Element&&);
void operator+=(ElementReference&, Element&&);  // addChild()

ElementReference setChild(ElementReference&, std::unique_ptr<xml::lite::Element>&&);  // destroyChildren() + addChild()
ElementReference setChild(ElementReference&, Element&&);  // destroyChildren() + addChild()

void setCharacterData(ElementReference&, const std::string&);
void setCharacterData(ElementReference&, const char*);
std::string getCharacterData(const ElementReference&);

}
}

#endif  // CODA_OSS_xml_easy_ElementReference_h_INCLUDED_
