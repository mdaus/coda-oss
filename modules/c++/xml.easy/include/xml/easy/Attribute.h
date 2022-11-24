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

#ifndef CODA_OSS_xml_easy_Attribute_h_INCLUDED_
#define CODA_OSS_xml_easy_Attribute_h_INCLUDED_
#pragma once

#include <string>
#include <vector>

#include "xml/lite/Attributes.h"

/*!
 *  \file Attributes.h
 *  \brief Implementation of SAX 2.0 Attributes
 *
 *  This file contains all of the elements needed to approximate the
 *  SAX 2.0 standard for Attributes.  The implementation is simpler here,
 *  and not all interfaces are supported.  However, the functions match
 *  the Attributes interface from the specification
 */

namespace xml
{
namespace easy
{

/*!
 *  \class AttributeNode
 *  \brief An internal holding structure for Attributes.
 *
 *  This provides the definition of an Attribute's
 *  internal organs.  We have a URI, a QName, and a local part
 *  as well.  We also need a value, of course.
 */
struct Attribute final
{
    Attribute() = default;
};

}
}

#endif // CODA_OSS_xml_lite_Attributes_h_INCLUDED_

