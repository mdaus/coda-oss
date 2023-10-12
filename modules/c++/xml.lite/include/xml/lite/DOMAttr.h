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
 * \file  DOMAttr.h
 * \brief File describing the tree API
 */

#include <memory>
#include <coda_oss/string.h>

#include <config/Exports.h>

#include "DOMNode.h"

namespace xml
{
namespace lite
{
struct AttributeNode;

/*!
 * \class DOMAttr
 * \brief Wrapper around AttributeNode that tries to follow
 * https://xerces.apache.org/xerces-c/ApacheDOMC++Binding.html
 */
class CODA_OSS_API DOMAttr final : public DOMNode
{
    AttributeNode* pAttributeNode = nullptr;

public:
    DOMAttr() = default;
    DOMAttr(AttributeNode&);
    ~DOMAttr() = default;

    DOMAttr(const DOMAttr&) = delete;
    DOMAttr& operator=(const DOMAttr&) = delete;
    DOMAttr(DOMAttr&&) = default;
    DOMAttr& operator=(DOMAttr&&) = default;

    /*!
     *  See DOMNode.hpp
     */
    Uri getNamespaceURI() const override;
    std::string getPrefix() const override;
    std::string getLocalName() const override;

    /*!
     *  See DOMAttr.hpp
     */
    std::string getName() const;
    std::string getValue() const;
    void setValue(const std::string&);
};

}
}


