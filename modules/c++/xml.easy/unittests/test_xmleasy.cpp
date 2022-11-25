/* =========================================================================
 * This file is part of xml.lite-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
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
#include <std/filesystem>
#include <std/optional>

#include "io/StringStream.h"
#include <TestCase.h>

#include "xml/lite/MinidomParser.h"
#include "xml/easy/Element.h"
#include "xml/easy/Attribute.h"


TEST_CASE(testXmlCreateRoot)
{
    xml::lite::MinidomParser xmlParser;
    auto& document = getDocument(xmlParser);

    xml::easy::Element documents(document.createElement(xml::lite::QName(xml::lite::Uri(), "abc"), "abc"));
    io::StringStream output;
    documents.celement().print(output);
    auto actual = output.stream().str();
    TEST_ASSERT_EQ("<abc>abc</abc>", actual);

    documents = "t";
    documents += "est";
    documents = xml::lite::QName(xml::lite::Uri(), "documents");

    output.reset();
    documents.celement().print(output);
    actual = output.stream().str();
    TEST_ASSERT_EQ("<documents>test</documents>", actual);
}

TEST_CASE(testXmlCreateNested)
{
    using E = xml::easy::Element;

    xml::lite::MinidomParser xmlParser;
    auto& document = getDocument(xmlParser);

    xml::easy::Element documents(document.createElement(xml::lite::QName(xml::lite::Uri(), "documents"), ""));
    documents = E("html");
    documents = E("html");

     io::StringStream output;
     documents.celement().print(output);
     auto actual = output.stream().str();
     const auto expected0 = "<documents><html/></documents>";
     TEST_ASSERT_EQ(expected0, actual);
     
      // xml::lite::AttributeNode a;
     // a.setQName("count");
     // a.setValue("1");
     // pDocuments->getAttributes().add(a);
     auto html = setChild(documents , "html");
     html += E(xml::lite::QName("title"), "Title");
     auto body = addChild(html, "body");
     auto p = addChild(body, "p");
     p = "paragraph";
     // a.setQName("a");
     // a.setValue("abc");
     // p.getAttributes().add(a);
     body += E("br");

    output.reset();
    documents.celement().print(output);
    actual = output.stream().str();
    const auto expected1 =
        "<documents>"
            "<html>"
                "<title>Title</title>"
                "<body>"
                    "<p>paragraph</p>"
                    "<br/>"
                "</body>"
             "</html>"
        "</documents>";
    TEST_ASSERT_EQ(expected1, actual);
}

int main(int, char**)
{
    TEST_CHECK(testXmlCreateRoot);
    TEST_CHECK(testXmlCreateNested);
}
