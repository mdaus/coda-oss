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
#include "xml/easy/ElementReference.h"

#include <tuple>

#include "xml/easy/Element.h"

xml::easy::ElementReference::ElementReference(xml::lite::Element& e) :
    element_(e)
{
}
xml::easy::ElementReference::ElementReference(Element& e) : ElementReference(e.element())
{
}

xml::lite::Element& xml::easy::ElementReference::ref()
{
    return element_;
}
const xml::lite::Element& xml::easy::ElementReference::ref() const
{
    return element_;
}

const xml::lite::Element& xml::easy::ElementReference::cref()
{
    return ref();
}
const xml::lite::Element& xml::easy::ElementReference::cref() const
{
    return ref();
}

xml::easy::ElementReference xml::easy::addChild(ElementReference& e, std::unique_ptr<xml::lite::Element>&& child)
{
    return e.ref().addChild(std::move(child));
}
void xml::easy::operator+=(ElementReference& e, std::unique_ptr<xml::lite::Element>&& child)
{
    std::ignore = addChild(e, std::move(child));
}
xml::easy::ElementReference xml::easy::addChild(ElementReference& e, Element&& child)
{
    return addChild(e, child.release());
}
void xml::easy::operator+=(ElementReference& e, Element&& child)
{
    std::ignore = addChild(e, std::move(child));
}

xml::easy::ElementReference xml::easy::setChild(ElementReference& e, std::unique_ptr<xml::lite::Element>&& child)
{
    e.ref().destroyChildren();
    return addChild(e, std::move(child));
}
xml::easy::ElementReference& xml::easy::ElementReference::operator=(std::unique_ptr<xml::lite::Element>&& child)
{
    setChild(*this, std::move(child));
    return *this;
}
xml::easy::ElementReference xml::easy::setChild(ElementReference& e, Element&& child)
{
    return setChild(e, child.release());
}
xml::easy::ElementReference& xml::easy::ElementReference::operator=(Element&& child)
{
    setChild(*this, std::move(child));
    return *this;
}

xml::easy::ElementReference& xml::easy::ElementReference::operator=(const std::string& characterData)
{
    setCharacterData(*this, characterData);
    return *this;
}
xml::easy::ElementReference& xml::easy::ElementReference::operator=(const char* characterData)
{
    *this = std::string(characterData);
    return *this;
}
xml::easy::ElementReference& xml::easy::ElementReference::operator=(const xml::lite::QName& qname)
{
    ref().setQName(qname);
    return *this;
}
xml::easy::ElementReference& xml::easy::ElementReference::operator=(const xml::lite::Uri& uri)
{
    ref().setUri(uri);
    return *this;
}

void xml::easy::setCharacterData(ElementReference& e, const std::string& s)
{
    e.ref().setCharacterData(s);
}
void xml::easy::setCharacterData(ElementReference& e, const char* s)
{
    setCharacterData(e, std::string(s));
}
std::string xml::easy::getCharacterData(const ElementReference& e)
{
    return e.ref().getCharacterData();
}