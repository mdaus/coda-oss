/* =========================================================================
 * This file is part of xml.lite-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * � Copyright 2023, Maxar Technologies, Inc.
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
#include <xml/lite/DOMImplementation.h>

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

    auto parser = xml::lite::DOMImplementation().createParser();
    auto doc = parser.parse(ss);

    auto root = doc.getDocumentElement();

    auto docElements = root.getElementsByTagName("doc");
    TEST_ASSERT_EQ(std::ssize(docElements), 1);

    auto aElements = getElementsByTagName(*docElements[0], "a");
    TEST_ASSERT_EQ(std::ssize(aElements), 1);
    auto& a = *(aElements[0]);

    auto characterData = getTextContent(a);
    TEST_ASSERT_EQ(str::to_native(characterData), text());

    setTextContent(a, str::u8FromNative(text()));
    characterData = getTextContent(a);
    TEST_ASSERT_EQ(str::to_native(characterData), text());
}

TEST_CASE(testXmlDOMWrite)
{
    auto document = xml::lite::DOMImplementation().createDocument();
    auto rootElement = document.createElementNS(xml::lite::QName(xml::lite::Uri(), "root"));

    const auto serializer = xml::lite::DOMImplementation().createSerializer();

    io::StringStream output;
    serializer.write(rootElement, output);
    const auto actual = output.stream().str();
    const auto expected = "<root/>";
    TEST_ASSERT_EQ(actual, expected);
}

static const std::string& strUri()
{
    static const std::string retval("urn:example.com");
    return retval;
}

static const auto& strAttributeXml()
{
    static const std::string strXml_1_ = R"(
<root>
    <doc name="doc">
        <a a="a">TEXT</a>
        <values int="314" double="3.14" string="abc" bool="yes" empty=""/>
        <ns_values xmlns:ns=")";
    static const std::string strXml_2_ = R"(" ns:int="314" />
    </doc>
</root>)";
    static const auto retval = strXml_1_ + strUri() + strXml_2_;
    return retval;
}

TEST_CASE(testXmlDOMGetAttribute)
{
    io::StringStream ss;
    ss.stream() << strAttributeXml();

    auto parser = xml::lite::DOMImplementation().createParser();
    auto document = parser.parse(ss);

    const auto& root = document.getDocumentElement();
    const auto& doc = getElementByTagName(root, "doc");
    const auto& a = getElementByTagName(*doc, "a");

    const auto value = a->getAttribute("a");
    TEST_ASSERT_EQ("a", *value);
}

TEST_CASE(testXmlDOMGetAttributeNode)
{
    io::StringStream ss;
    ss.stream() << strAttributeXml();

    auto parser = xml::lite::DOMImplementation().createParser();
    auto document = parser.parse(ss);

    auto root = document.getDocumentElement();
    auto doc = getElementByTagName(root, "doc");
    auto a = getElementByTagName(*doc, "a");

    auto node = a->getAttributeNode("asdf");
    TEST_ASSERT(!node.has_value());

    node = a->getAttributeNode("a");
    TEST_ASSERT(node.has_value());
    const auto value = node->getValue();
    TEST_ASSERT_EQ("a", value);
}

TEST_MAIN(
    TEST_CHECK(testXmlDOMParse);
    TEST_CHECK(testXmlDOMWrite);
    TEST_CHECK(testXmlDOMGetAttribute);
)