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

#include <std/string>
#include <std/span> // std::ssize()

#include <TestCase.h>

#include <str/Convert.h>
#include <xml/lite/DOMParser.h>
#include <xml/lite/DOMDocument.h>
#include <xml/lite/DOMSerializer.h>

static const std::string& text()
{
    static const std::string retval("TEXT");
    return retval;
}
static const std::string& strXml()
{
    static const std::string retval = "<root><doc><a>" + text() + "</a></doc></root>";
    return retval;
}

TEST_CASE(testXmlDOMParse)
{
    io::StringStream ss;
    ss.stream() << strXml();
    TEST_ASSERT_EQ(ss.stream().str(), strXml());

    xml::lite::DOMParser parser;
    auto doc = parser.parse(ss);

    const auto& root = doc.getDocumentElement();

    const auto docElements = root.getElementsByTagName("doc");
    TEST_ASSERT_EQ(std::ssize(docElements), 1);

    const auto aElements = getElementsByTagName(*docElements[0], "a");
    TEST_ASSERT_EQ(std::ssize(aElements), 1);
    auto& a = *(aElements[0]);

    auto characterData = a.getNodeValue();
    TEST_ASSERT_EQ(str::to_native(characterData), text());

    a.setNodeValue(str::u8FromNative(text()));
    characterData = a.getNodeValue();
    TEST_ASSERT_EQ(str::to_native(characterData), text());
}

TEST_CASE(testXmlDOMWrite)
{
    xml::lite::DOMDocument document;
    auto rootElement = document.createElementNS("" /*uri*/, "root");

    const xml::lite::DOMSerializer serializer;

    io::StringStream output;
    serializer.write(rootElement, output);
    //expected = "<root>" + characterData + "</root>";
}


TEST_MAIN(
    TEST_CHECK(testXmlDOMParse);
    TEST_CHECK(testXmlDOMWrite);
)
