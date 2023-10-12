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

#include <memory>

#include <config/Exports.h>

#include "QName.h"
#include "DOMParser.h"
#include "DOMSerializer.h"
#include "DOMConfiguration.h"
#include "DOMDocument.h"

/*!
 * \file DOMImplementation.h
 * https://xerces.apache.org/xerces-c/ApacheDOMC++Binding.html
 *
 */

namespace xml
{
namespace lite
{
/*!
 * \class DOMImplementation
 *
 */
struct CODA_OSS_API DOMImplementation final
{
    DOMImplementation();
    ~DOMImplementation();

    /*!
     *  See DOMImplementation.hpp
     */
    DOMDocument createDocument() const;
    DOMDocument createDocument(const QName&) const;
    DOMDocument createDocument(const Uri&, const std::string&) const;

    /*!
     *  See DOMImplementationLS.hpp
     */
    DOMParser createParser() const;
    DOMSerializer createSerializer() const;

private:
    // We want a new DOMConfiguration for each DOMImplementation, but
    // we want that configuration to be *shared* among everything created
    // via the factory function.  Furthermore, we don't want to force clients
    // to keep an DOMImplementation instance around, it's convenient
    // to write `auto parser = DOMImplementation().createParser();`.
    std::shared_ptr<DOMConfiguration> pConfiguration;
};

}
}
