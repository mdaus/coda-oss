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
#include "xml/lite/ElementReference.h"

#include <tuple>

xml::lite::ElementReference::ElementReference(const Element& e) : element_(e)
{
}

const xml::lite::Element& xml::lite::ElementReference::ref() const
{
    return element_;
}

const xml::lite::Element& xml::lite::ElementReference::cref()
{
    return ref();
}
const xml::lite::Element& xml::lite::ElementReference::cref() const
{
    return ref();
}

xml::lite::ElementMutableReference::ElementMutableReference(Element& e) : ElementReference(e), element_(e)
{
}

xml::lite::Element& xml::lite::ElementMutableReference::ref()
{
    return element_;
}

xml::lite::ElementMutableReference xml::lite::addChild(ElementMutableReference& e, std::unique_ptr<Element>&& child)
{
    return e.ref().addChild(std::move(child));
}
void xml::lite::operator+=(ElementMutableReference& e, std::unique_ptr<xml::lite::Element>&& child)
{
    std::ignore = addChild(e, std::move(child));
}

xml::lite::ElementMutableReference xml::lite::setChild(ElementMutableReference& e, std::unique_ptr<Element>&& child)
{
    e.ref().destroyChildren();
    return addChild(e, std::move(child));
}
xml::lite::ElementMutableReference& xml::lite::ElementMutableReference::operator=(std::unique_ptr<Element>&& child)
{
    std::ignore = setChild(*this, std::move(child));
    return *this;
}

xml::lite::ElementMutableReference& xml::lite::ElementMutableReference::operator=(const std::string& characterData)
{
    setCharacterData(*this, characterData);
    return *this;
}
xml::lite::ElementMutableReference& xml::lite::ElementMutableReference::operator=(const char* characterData)
{
    *this = std::string(characterData);
    return *this;
}
xml::lite::ElementMutableReference& xml::lite::ElementMutableReference::operator=(const QName& qname)
{
    ref().setQName(qname);
    return *this;
}
xml::lite::ElementMutableReference& xml::lite::ElementMutableReference::operator=(const Uri& uri)
{
    ref().setUri(uri);
    return *this;
}

void xml::lite::setCharacterData(ElementMutableReference& e, const std::string& s)
{
    e.ref().setCharacterData(s);
}
void xml::lite::setCharacterData(ElementMutableReference& e, const char* s)
{
    setCharacterData(e, std::string(s));
}
std::string xml::lite::getCharacterData(const ElementReference& e)
{
    return e.ref().getCharacterData();
}