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

#include <assert.h>

xml::easy::Element::Element(std::unique_ptr<xml::lite::Element>&& element) :
    element_(std::move(element))
{
}
xml::easy::Element::Element(xml::lite::Element& element) : pElement_(&element)
{
}

xml::easy::Element::Element(std::string qname) :
    element_(xml::lite::Element::create(qname))
{
}
xml::easy::Element::Element(const char* qname) : Element(std::string(qname))
{
}

xml::easy::Element::Element(const xml::lite::QName& qname,
                            std::string characterData) :
    element_(xml::lite::Element::create(qname, std::move(characterData)))
{
}

const xml::lite::Element& xml::easy::Element::element() const
{
    if (pElement_ != nullptr)
    {
        assert(element_.get() == nullptr);
        return *pElement_;
    }

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
    setChild(std::move(child));
    return *this;
}

xml::easy::Element& xml::easy::Element::operator=(std::string characterData)
{
    element().setCharacterData(std::move(characterData));
    return *this;
}
xml::easy::Element& xml::easy::Element::operator=(const char* characterData)
{
    *this = std::string(characterData);
    return *this;
}
void xml::easy::operator+=(Element& e, std::string characterData)
{
    e = e.celement().getCharacterData() + characterData;
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

xml::easy::Element xml::easy::Element::addChild(Element&& child)
{
    auto pChild = child.element_.get();
    element().addChild(std::move(child.element_));
    return Element(*pChild);
}
xml::easy::Element xml::easy::Element::setChild(Element&& child)
{
    element().destroyChildren();
    return addChild(std::move(child));
}
