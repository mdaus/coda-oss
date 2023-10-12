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

#pragma once

/*!
 * \file  DOMDocument.h
 * \brief File describing the tree API
 *
 * This is a simple alternative to the DOM API.  It is very similar,
 * but is a solution intended more for speed and simplicity.  The
 * DOM API offers far more functionality, where this version implements
 * only the widely used features.  In this file, we are only concerned
 * with the data structure for implementing the XML reader, not the reader
 * itself.
 */

#include <assert.h>

#include <memory>
#include <coda_oss/string.h>
#include <coda_oss/optional.h>
#include <coda_oss/span.h>

#include <config/Exports.h>

#include "DOMNode.h"
#include "DOMAttr.h"
#include "Element.h"
#include "QName.h"

namespace xml
{
namespace lite
{

/*!
 * \class DOMElement
 * \brief Wrapper around Element that tries to follow
 * https://xerces.apache.org/xerces-c/ApacheDOMC++Binding.html
 */
struct CODA_OSS_API DOMElement final : public DOMNode
{
    DOMElement(Element&);
    DOMElement(std::unique_ptr<Element>);
    ~DOMElement();

    DOMElement(const DOMElement&) = delete;
    DOMElement& operator=(const DOMElement&) = delete;
    DOMElement(DOMElement&&) = default;
    DOMElement& operator=(DOMElement&&) = default;

    /*!
     *  See DOMNode.hpp
     */
    std::string getNodeName() const override;
    Uri getNamespaceURI() const override;

    /*!
     *  See DOMElement.hpp
     */
    coda_oss::optional<std::string> getAttribute(const std::string&) const;
    coda_oss::optional<DOMAttr> getAttributeNode(const std::string&);
    DOMConstNodeList getElementsByTagName(const std::string& tag) const;
    DOMMutableNodeList getElementsByTagName(const std::string& tag);
    void setAttribute(const std::string& name, const std::string& value);
    // setAttributeNode()
    // removeAttributeNode()
    // removeAttribute()
    coda_oss::optional<std::string> getAttributeNS(const QName&) const;
    void setAttributeNS(const QName&, const std::string& value);
    // removeAttributeNS()
    coda_oss::optional<DOMAttr> getAttributeNodeNS(const QName&);
    // setAttributeNodeNS()
    DOMConstNodeList getElementsByTagNameNS(const QName&) const;
    DOMMutableNodeList getElementsByTagNameNS(const QName&);

    Element& details_getElement_()
    {
        return getElement();
    }
    const Element& details_getElement_() const
    {
        return getElement();
    }

private:
    Element& getElement();
    const Element& getElement() const;

    Element* pElement_ = nullptr;
    std::unique_ptr<Element> pOwnedElement_;
};

inline auto getElementsByTagName(const DOMNode& node, const std::string& tag)
{
    return dynamic_cast<const xml::lite::DOMElement&>(node).getElementsByTagName(tag);
}
inline auto getElementsByTagName(DOMNode& node, const std::string& tag)
{
    return dynamic_cast<xml::lite::DOMElement&>(node).getElementsByTagName(tag);
}

inline std::unique_ptr<const DOMNode> getNodeByTagName(const DOMElement& element, const std::string& tag)
{
    auto elements = getElementsByTagName(element, tag);
    assert(coda_oss::ssize(elements) == 1);
    return std::move(elements[0]);
}
inline std::unique_ptr<DOMNode> getNodeByTagName(DOMElement& element, const std::string& tag)
{
    auto elements = getElementsByTagName(element, tag);
    assert(coda_oss::ssize(elements) == 1);
    return std::move(elements[0]);
}

inline auto getElementByTagName(const DOMElement& element, const std::string& tag)
{
    auto node = getNodeByTagName(element, tag);

    std::unique_ptr<const DOMElement> retval;
    if (dynamic_cast<const DOMElement*>(node.get())) // right type?
    {
        retval.reset(dynamic_cast<const DOMElement*>(node.release())); // transfer ownership
    }
    return retval;
}
inline auto getElementByTagName(DOMElement& element, const std::string& tag)
{
    auto node = getNodeByTagName(element, tag);

    std::unique_ptr<DOMElement> retval;
    if (dynamic_cast<DOMElement*>(node.get())) // right type?
    {
        retval.reset(dynamic_cast<DOMElement*>(node.release())); // transfer ownership
    }
    return retval;
}

// The DOM documentation (https://xerces.apache.org/xerces-c/apiDocs-3/classDOMNode.html) says that `nodeValue` should
// be `null` for *DOMElement*.  It also says that the text is stored as a single *DOMText* node.
// However, that API doesn't line-up very well with our existing **xml.lite** code; instead, we'll
// "overload" `getTextContent()` and `setTextContent()`.  Note that these are intentionally
// free functions, not the overridden *DOMNode* methods.
CODA_OSS_API coda_oss::u8string getTextContent(const DOMElement&);
inline coda_oss::u8string getTextContent(const DOMNode& node)
{
    return getTextContent(dynamic_cast<const DOMElement&>(node));
}
CODA_OSS_API void setTextContent(DOMElement&, const coda_oss::u8string&);
inline void setTextContent(DOMNode& node, const coda_oss::u8string& v)
{
    setTextContent(dynamic_cast<DOMElement&>(node), v);
}

}
}


