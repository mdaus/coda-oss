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

#include "xml/lite/MinidomParser.h"

#include <stdexcept>

xml::lite::MinidomParser::MinidomParser(bool storeEncoding)
{
    if (!storeEncoding)
    {
        throw std::invalid_argument("'storeEncoding' is no longer used and must always be 'true'");
    }
    mReader.setContentHandler(&mHandler);
}

void xml::lite::MinidomParser::parse(io::InputStream& is, int size)
{
    mReader.parse(is, size);
}
void xml::lite::MinidomParser::parse(io::InputStream& is, const void*pInitialEncoding, const void* pFallbackEncoding, int size)
{
    mReader.parse(is, pInitialEncoding, pFallbackEncoding, size);
}

void xml::lite::MinidomParser::clear()
{
    mHandler.clear();
}

xml::lite::Document* xml::lite::MinidomParser::getDocument() const
{
    return mHandler.getDocument();
}

xml::lite::Document* xml::lite::MinidomParser::getDocument(bool steal)
{
    return mHandler.getDocument(steal);
}
std::unique_ptr<xml::lite::Document>& xml::lite::MinidomParser::getDocument(std::unique_ptr<Document>& pDocument)
{
    return mHandler.getDocument(pDocument);
}

void xml::lite::MinidomParser::setDocument(xml::lite::Document* newDocument, bool own)
{
    mHandler.setDocument(newDocument, own);
}
void xml::lite::MinidomParser::setDocument(std::unique_ptr<Document>&& newDocument)
{
    mHandler.setDocument(std::move(newDocument));
}

void xml::lite::MinidomParser::preserveCharacterData(bool preserve)
{
    mHandler.preserveCharacterData(preserve);
}
