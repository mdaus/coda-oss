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

#ifndef CODA_OSS_xml_easy_Element_h_INCLUDED_
#define CODA_OSS_xml_easy_Element_h_INCLUDED_
#pragma once

#include <coda_oss/string.h>

#include <memory>
#include <string>

#include "xml/easy/Attribute.h"
#include "xml/lite/Element.h"
#include "xml/lite/QName.h"

#include "xml/easy/ElementReference.h"

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

struct Element final
{
    Element(std::unique_ptr<xml::lite::Element>&& element);

    Element(const std::string& qname);
    Element(const char* qname);
    Element(const xml::lite::QName&, const std::string& characterData);

    Element(const Element&) = delete;
    Element& operator=(const Element&) = delete;
    Element(Element&&) = default;
    Element& operator=(Element&&) noexcept(false);  // setChild()

    ~Element() = default;

    xml::lite::Element& element();
    const xml::lite::Element& celement();
    const xml::lite::Element& element() const;

    Element& operator=(const std::string&);  // setCharacterData()
    Element& operator=(const char*);  // setCharacterData()
    Element& operator=(const xml::lite::QName&);  // setQName()
    Element& operator=(const xml::lite::Uri&);  // setUri()

    void operator+=(Element&&); // addChild()

    Element operator[](std::string) const;

    ElementMutableReference addChild(Element&&);

    operator ElementReference() const;
    operator ElementMutableReference();

    std::unique_ptr<xml::lite::Element>&& release()
    {
        return std::move(element_);
    }

private:
    std::unique_ptr<xml::lite::Element> element_;
};

void operator+=(Element&, const std::string&);
void operator+=(Element&, const char*);

ElementMutableReference setChild(Element&, Element&&);  // destroyChildren() + addChild()

void setCharacterData(Element&, const std::string&);
void setCharacterData(Element&, const char*);
std::string getCharacterData(const Element&);

}
}

#endif  // CODA_OSS_xml_easy_Element_h_INCLUDED_
