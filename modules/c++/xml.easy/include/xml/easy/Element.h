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
    Element(xml::lite::Element&);

    Element(std::string qname);
    Element(const char* qname);
    Element(const xml::lite::QName&, std::string characterData);

    Element(const Element&) = delete;
    Element& operator=(const Element&) = delete;
    Element(Element&&) = default;
    Element& operator=(Element&&) noexcept(false);  // setChild()

    ~Element() = default;

    xml::lite::Element& element();
    const xml::lite::Element& celement();
    const xml::lite::Element& element() const;

    Element& operator=(std::string);  // setCharacterData()
    Element& operator=(const char*);  // setCharacterData()
    Element& operator=(const xml::lite::QName&);  // setQName()
    Element& operator=(const xml::lite::Uri&);  // setUri()

    void operator+=(Element&&); // addChild()

    Element operator[](std::string) const;

    Element addChild(Element&&);
    Element setChild(Element&&);  // destroyChildren() + addChild()

private:
    std::unique_ptr<xml::lite::Element> element_;
    xml::lite::Element* pElement_ = nullptr;
};

void operator+=(Element&, std::string);
void operator+=(Element&, const char*);

}
}

#endif  // CODA_OSS_xml_easy_Element_h_INCLUDED_
