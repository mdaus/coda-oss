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

#include "xml/lite/DOMSerializer.h"


#include <assert.h>

#include <io/StringStream.h>

#include "xml/lite/DOMElement.h"

xml::lite::DOMConfiguration& xml::lite::DOMSerializer::getDomConfig()
{
    return configuration;
}

bool xml::lite::DOMSerializer::write(const DOMNode& node, io::OutputStream& os) const
{
    if (auto pElement = dynamic_cast<const DOMElement*>(&node))
    {
        pElement->pElement_->print(os);
        return true;
    }

    return false; // node isn't DOMElement
}

coda_oss::u8string xml::lite::DOMSerializer::writeToString(const DOMNode& node) const
{
    io::U8StringStream ss;
    if (write(node, ss))
    {
        return ss.stream().str();
    }

    return coda_oss::u8string{};
}