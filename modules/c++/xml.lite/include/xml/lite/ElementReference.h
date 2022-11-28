/* =========================================================================
 * This file is part of xml.lite-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

#ifndef CODA_OSS_xml_lite_ElementReference_h_INCLUDED_
#define CODA_OSS_xml_lite_ElementReference_h_INCLUDED_
#pragma once

#include <coda_oss/string.h>

#include <memory>
#include <string>

#include "xml/lite/Element.h"
#include "xml/lite/QName.h"

namespace xml
{
namespace lite
{

/*!
 * \class Element
 * \brief The class defining one element of an XML document
 *
 * This class stores all of the element information about an XML
 * document.
 */
struct ElementReference
{
    ElementReference(const Element&);
    ElementReference(const ElementReference&) = delete;
    ElementReference& operator=(const ElementReference&) = delete;
    ElementReference(ElementReference&&) = default;
    ElementReference& operator=(ElementReference&&) = default;
    ~ElementReference() = default;

    const Element& ref() const;
    const Element& cref();
    const Element& cref() const;

private:
    const Element& element_;
};

std::string getCharacterData(const ElementReference&);

struct ElementMutableReference final : public ElementReference
{
    ElementMutableReference(Element&);
    ElementMutableReference(const ElementMutableReference&) = delete;
    ElementMutableReference& operator=(const ElementMutableReference&) = delete;
    ElementMutableReference(ElementMutableReference&&) = default;
    ElementMutableReference& operator=(ElementMutableReference&&) = default;
    ~ElementMutableReference() = default;

    Element& ref();

    ElementMutableReference& operator=(const std::string&);  // setCharacterData()
    ElementMutableReference& operator=(const char*);  // setCharacterData()
    ElementMutableReference& operator=(const QName&);  // setQName()
    ElementMutableReference& operator=(const Uri&);  // setUri()
    ElementMutableReference& operator=(std::unique_ptr<Element>&&);  // setChild()

private:
    Element& element_;
};

ElementMutableReference addChild(ElementMutableReference&, std::unique_ptr<Element>&&);
void operator+=(ElementMutableReference&, std::unique_ptr<Element>&&);  // addChild()
ElementMutableReference addChild(ElementMutableReference&, Element&&);

ElementMutableReference setChild(ElementMutableReference&, std::unique_ptr<Element>&&);  // destroyChildren() + addChild()

void setCharacterData(ElementMutableReference&, const std::string&);
void setCharacterData(ElementMutableReference&, const char*);

}
}

#endif  // CODA_OSS_xml_lite_ElementReference_h_INCLUDED_
