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

#include <memory>
#include <string>
#include <new> // std::nothrow_t
#include <coda_oss/string.h>

#include "xml/lite/Attributes.h"
#include "xml/lite/QName.h"
#include "xml/lite/Element.h"

/*!
 * \file  Element.h
 * \brief This contains the xml::lite data type for elements.
 *
 * This class is analogous to its DOM namesake.  It contains all of
 * the information that would be expected of it, including comment data.
 * The API, however, is much simpler and less developed than DOM.
 */

// xml::lite::Element
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
    Element() = delete;

    Element(std::unique_ptr<xml::lite::Element>&& element);

    Element(std::string qname);
    Element(const xml::lite::QName&, std::string characterData);

    Element(const Element&) = delete;
    Element& operator=(const Element&) = delete;
    Element(Element&&) = default;
    Element& operator=(Element&&) = default;

    ~Element() = default;

    xml::lite::Element& element();
    const xml::lite::Element& celement();
    const xml::lite::Element& element() const;

    Element& operator=(std::string);
    Element& operator=(const xml::lite::QName&);

     Element& operator+=(Element&&);

     Element operator[](std::string) const;

private:
    std::unique_ptr<xml::lite::Element> element_;
};
  
}
}

#endif // CODA_OSS_xml_easy_Element_h_INCLUDED_
