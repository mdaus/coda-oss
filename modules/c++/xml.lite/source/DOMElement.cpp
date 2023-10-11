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

#include "xml/lite/DOMElement.h"

xml::lite::DOMElement::DOMElement(Element& element) : pElement_(&element)
{
}
xml::lite::DOMElement::DOMElement(std::unique_ptr<Element> pElement) : pOwnedElement_(std::move(pElement))
{
}
xml::lite::DOMElement::~DOMElement() = default;

template<typename T1, typename T2>
inline auto& getElement_(T1&& pElement_, T2&& pOwnedElement_)
{
    if (pElement_ == nullptr)
    {
        assert(pOwnedElement_.get() != nullptr);
        return *pOwnedElement_;
    }

    assert(pOwnedElement_.get() == nullptr);
    return *pElement_;
}
xml::lite::Element& xml::lite::DOMElement::getElement()
{
    return getElement_(pElement_, pOwnedElement_);
}
const xml::lite::Element& xml::lite::DOMElement::getElement() const
{
    return getElement_(pElement_, pOwnedElement_);
}

coda_oss::u8string xml::lite::DOMElement::getNodeValue() const
{
    return getCharacterData(getElement());
}

void xml::lite::DOMElement::setNodeValue(const coda_oss::u8string& v)
{
    getElement().setCharacterData(v);
}

xml::lite::DOMNodeList xml::lite::DOMElement::getElementsByTagName(const std::string& tag) const
{
    const auto elements = getElement().getElementsByTagName(tag);

    xml::lite::DOMNodeList retval;
    for (auto& element : elements)
    {
        retval.emplace_back(std::make_unique<DOMElement>(*element));
    }
    return retval;
}
