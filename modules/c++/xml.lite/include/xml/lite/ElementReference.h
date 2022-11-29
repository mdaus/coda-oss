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
 * \class ElementReference and ElementMutableReference
 * \brief Non-ownning wrappers around an Element inststance.
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

coda_oss::u8string getCharacterData(const ElementReference&);
std::string& getCharacterData(const ElementReference&, std::string& result);

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

ElementMutableReference setChild(ElementMutableReference&, std::unique_ptr<Element>&&);  // destroyChildren() + addChild()

ElementMutableReference addChild(ElementMutableReference&, std::unique_ptr<Element>&&);
void operator+=(ElementMutableReference&, std::unique_ptr<Element>&&);  // addChild()

ElementMutableReference addChild(ElementMutableReference&, const std::string& qname);
ElementMutableReference addChild(ElementMutableReference&, const QName&);
ElementMutableReference addChild(ElementMutableReference&, const std::string& qname, const coda_oss::u8string& characterData);
ElementMutableReference addChild(ElementMutableReference&, const std::string&, const std::string&) = delete; // NO, order matters!
ElementMutableReference addChild(ElementMutableReference&, const QName&, const coda_oss::u8string& characterData);
ElementMutableReference addChild(ElementMutableReference&, const QName&, const std::string& characterData);
ElementMutableReference addChild(ElementMutableReference&, const std::string& qname, const Uri&);
ElementMutableReference addChild(ElementMutableReference&, const std::string& qname, const Uri&, const coda_oss::u8string& characterData);
ElementMutableReference addChild(ElementMutableReference&, const std::string& qname, const coda_oss::u8string& characterData, const Uri&);

void operator+=(ElementMutableReference&, const std::string& qname);  // addChild()

void setCharacterData(ElementMutableReference&, const coda_oss::u8string&);
void setCharacterData(ElementMutableReference&, coda_oss::u8string::const_pointer);
void setCharacterData(ElementMutableReference&, const std::string&);
void setCharacterData(ElementMutableReference&, std::string::const_pointer);

}
}

#endif  // CODA_OSS_xml_lite_ElementReference_h_INCLUDED_
