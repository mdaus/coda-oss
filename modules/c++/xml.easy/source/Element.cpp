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
#include "xml/easy/Element.h"

#include "xml/easy/ElementReference.h"

xml::easy::Element::Element(std::unique_ptr<xml::lite::Element>&& element) :
    element_(std::move(element))
{
}

xml::easy::Element::Element(const std::string& qname) :
    element_(xml::lite::Element::create(qname))
{
}
xml::easy::Element::Element(const char* qname) : Element(std::string(qname))
{
}

xml::easy::Element::Element(const xml::lite::QName& qname, const std::string& characterData) :
    element_(xml::lite::Element::create(qname, characterData))
{
}

xml::easy::Element::operator xml::easy::ElementReference() const
{
    return ElementReference(*this);
}
xml::easy::Element::operator xml::easy::ElementMutableReference()
{
    return ElementMutableReference(*this);
}

const xml::lite::Element& xml::easy::Element::element() const
{
    return *element_;
}
xml::lite::Element& xml::easy::Element::element()
{
    const xml::easy::Element* pThis = this; 
    return const_cast<xml::lite::Element&>(pThis->element()); // C++23 might fix this
}
const xml::lite::Element& xml::easy::Element::celement()
{
    return element();
}

xml::easy::Element& xml::easy::Element::operator=(Element&& child) noexcept(false)
{
    setChild(*this, std::move(child));
    return *this;
}

xml::easy::Element& xml::easy::Element::operator=(const std::string& characterData)
{
    setCharacterData(*this, characterData);
    return *this;
}
xml::easy::Element& xml::easy::Element::operator=(const char* characterData)
{
    *this = std::string(characterData);
    return *this;
}
void xml::easy::operator+=(Element& e, const std::string& characterData)
{
    e = getCharacterData(e) + characterData;
}
void xml::easy::operator+=(Element& e, const char* characterData)
{
    e += std::string(characterData);
}

xml::easy::Element& xml::easy::Element::operator=(const xml::lite::QName& qname)
{
    element().setQName(qname);
    return *this;
}
xml::easy::Element& xml::easy::Element::operator=(const xml::lite::Uri& uri)
{
    element().setUri(uri);
    return *this;
}

void xml::easy::Element::operator+=(xml::easy::Element&& child)
{
    addChild(std::move(child));
}

// xml::easy::Element xml::easy::Element::operator[](std::string localName)
// const
//{
//     auto& e = element().getElementByTagName(localName);
// }

xml::easy::ElementMutableReference xml::easy::Element::addChild(Element&& child)
{
    ElementMutableReference ref(*this);
    return xml::easy::addChild(ref, std::move(child));
}
xml::easy::ElementMutableReference xml::easy::setChild(Element& e, Element&& child)
{
    ElementMutableReference ref(e);
    return setChild(ref, std::move(child));
}

void xml::easy::setCharacterData(Element& e, const std::string& s)
{
    e.element().setCharacterData(s);
}
void xml::easy::setCharacterData(Element& e, const char* s)
{
    setCharacterData(e, std::string(s));
}
std::string xml::easy::getCharacterData(const Element& e)
{
    return e.element().getCharacterData();
}