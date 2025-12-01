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

#include <string>

#include "io/StringStream.h"
#include <catch2/catch_test_macros.hpp>

#include "xml/lite/MinidomParser.h"
#include "xml/lite/QName.h"

static const std::string& strUri()
{
    static const std::string retval("urn:example.com");
    return retval;
}
static const xml::lite::Uri& uri()
{
    static const xml::lite::Uri retval(strUri());
    return retval;
}

static const auto& strXml()
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

struct test_MinidomParser final
{
    xml::lite::MinidomParser xmlParser;
    xml::lite::Element* getRootElement()
    {
        io::StringStream ss;
        ss.stream() << strXml();

        xmlParser.parse(ss);
        return getDocument(xmlParser).getRootElement();
    }
};

TEST_CASE("test_getAttribute")
{
    using namespace xml::lite::literals;  // _q and _u for QName and Uri

    test_MinidomParser xmlParser;
    const auto root = xmlParser.getRootElement();

    const auto& a = root->getElementByTagName("a", true /*recurse*/);
    const auto& attributes = a.getAttributes();

    std::string value;
    value = attributes.getValue("a");
    CHECK("a" == value);
    value = attributes["a"];
    CHECK("a" == value);
    value = attributes["a"_q];
    CHECK("a" == value);

    const auto result = attributes.getValue("a", value);
    CHECK(result);
    CHECK("a" == value);
}

TEST_CASE("test_getAttributeByNS")
{
    test_MinidomParser xmlParser;
    const auto root = xmlParser.getRootElement();

    const auto& e = root->getElementByTagName("ns_values", true /*recurse*/);
    const auto& attributes = e.getAttributes();

    using namespace xml::lite;

    std::string strValue;
    strValue = attributes.getValue(xml::lite::QName(uri(), "int"));
    CHECK("314" == strValue);
    strValue = getValue<std::string>(attributes, uri(), "int");
    CHECK("314" == strValue);
    const auto key = xml::lite::QName(uri(), "int");
    strValue = getValue<std::string>(attributes, key);
    CHECK("314" == strValue);

    int value;
    auto result = getValue<int>(attributes, uri(), "int", value);
    CHECK(result);
    CHECK(314 == value);
    value = getValue<int>(attributes, uri(), "int");
    CHECK(314 == value);

    result = getValue<int>(attributes, key, value);
    CHECK(result);
    CHECK(314 == value);
    value = getValue<int>(attributes, key);
    CHECK(314 == value);
}

TEST_CASE("test_getAttributeByIndex")
{
    test_MinidomParser xmlParser;
    const auto root = xmlParser.getRootElement();

    const auto& a = root->getElementByTagName("a", true /*recurse*/);
    const auto& attributes = a.getAttributes();

    std::string value;
    value = attributes.getValue(0);
    CHECK("a" == value);

    const auto result = attributes.getValue(0, value);
    CHECK(result);
    CHECK("a" == value);
}

TEST_CASE("test_getAttributeNotFound")
{
    test_MinidomParser xmlParser;
    const auto root = xmlParser.getRootElement();

    const auto& a = root->getElementByTagName("a", true /*recurse*/);
    const auto& attributes = a.getAttributes();

    std::string value;
    auto result = attributes.getValue("not_found", value);
    CHECK_FALSE(result);
    result = attributes.getValue(999, value);
    CHECK_FALSE(result);

    CHECK_THROWS_AS(attributes.getValue("not_found"), except::NoSuchKeyException);        
    CHECK_THROWS(attributes.getValue(999));
}

TEST_CASE("test_getAttributeValue")
{
    test_MinidomParser xmlParser;
    const auto root = xmlParser.getRootElement();

    const auto& values = root->getElementByTagName("values", true /*recurse*/);
    const auto& attributes = values.getAttributes();

    using namespace xml::lite;
    {
        int value;
        const auto result = getValue(attributes, "int", value);
        CHECK(result);
        CHECK(314 == value);

        value = getValue<int>(attributes, "int");
        CHECK(314 == value);
    }
    {
        double value;
        const auto result = getValue(attributes, "double", value);
        CHECK(result);
        CHECK(3.14 == value);

        value = getValue<double>(attributes, "double");
        CHECK(3.14 == value);
    }
    {
        std::string value;
        const auto result = getValue(attributes, "string", value);
        CHECK(result);
        CHECK("abc" == value);

        value = getValue<std::string>(attributes, "string");
        CHECK("abc" == value);
    }
    {
        std::string value;
        const auto result = getValue(attributes, "empty", value);
        CHECK_FALSE(result);
    }
    {
        auto toType = [](const std::string& value) { return value == "yes"; };
        bool value = false;
        auto result = castValue(attributes, "bool", value, toType);
        CHECK(result);
        CHECK(true == value);

        value = castValue(attributes, "bool", toType);
        CHECK(true == value);

        std::string strValue;
        result = getValue(attributes, "bool", strValue);
        CHECK(result);
        CHECK("yes" == strValue);

        strValue = getValue<std::string>(attributes, "bool");
        CHECK("yes" == strValue);
    }
    {
        std::string value;
        const auto result = getValue(attributes, "not_found", value);
        CHECK_FALSE(result);
    }
}

TEST_CASE("test_getAttributeValueFailure")
{
    test_MinidomParser xmlParser;
    const auto root = xmlParser.getRootElement();

    const auto& values = root->getElementByTagName("values", true /*recurse*/);
    const auto& attributes = values.getAttributes();

    {
        int value;
        const auto result = getValue(attributes, "string", value);
        CHECK_FALSE(result);
    }
    {
        double value;
        const auto result = getValue(attributes, "string", value);
        CHECK_FALSE(result);
    }
    {
        std::string value;
        const auto result = getValue(attributes, "empty", value);
        CHECK_FALSE(result);
        value = attributes.getValue("empty");
        CHECK(value.empty());
    }
}

TEST_CASE("test_getAttributeValueThrows")
{
    test_MinidomParser xmlParser;
    const auto root = xmlParser.getRootElement();

    const auto& values = root->getElementByTagName("values", true /*recurse*/);
    const auto& attributes = values.getAttributes();

    using namespace xml::lite;
    CHECK_THROWS_AS(getValue<int>(attributes, "string"), except::BadCastException);
    CHECK_THROWS_AS(getValue<double>(attributes, "string"), except::BadCastException);
    CHECK_THROWS_AS(getValue<std::string>(attributes, "empty"), except::BadCastException);

    CHECK_THROWS_AS(getValue<std::string>(attributes, "not_found"), except::NoSuchKeyException);
}

TEST_CASE("test_getAttributeValueByIndex")
{
    test_MinidomParser xmlParser;
    const auto root = xmlParser.getRootElement();

    const auto& values = root->getElementByTagName("values", true /*recurse*/);
    const auto& attributes = values.getAttributes();

    using namespace xml::lite;
    {
        int value;
        const auto result = getValue(attributes, 0, value);
        CHECK(result);
        CHECK(314 == value);

        value = getValue<int>(attributes, 0);
        CHECK(314 == value);
    }
    {
        double value;
        const auto result = getValue(attributes, 1, value);
        CHECK(result);
        CHECK(3.14 == value);
        
        value = getValue<double>(attributes, 1);
        CHECK(3.14 == value);
    }
    {
        std::string value;
        const auto result = getValue(attributes, 2, value);
        CHECK(result);
        CHECK("abc" == value);

        value = getValue<std::string>(attributes, 2);
        CHECK("abc" == value);
    }
    {
        std::string value;
        auto result = getValue(attributes, -1, value);
        CHECK_FALSE(result);
        CHECK_THROWS_AS(getValue<std::string>(attributes, -1), except::NoSuchKeyException);

        result = getValue(attributes, 999, value);
        CHECK_FALSE(result);
        CHECK_THROWS_AS(getValue<std::string>(attributes, 999), except::NoSuchKeyException);
    }
}

TEST_CASE("test_setAttributeValue")
{
    test_MinidomParser xmlParser;
    auto root = xmlParser.getRootElement();

    auto& values = root->getElementByTagName("values", true /*recurse*/);
    auto& attributes = values.getAttributes();

    {
        auto result = setValue(attributes, "int", 123);
        CHECK(result);
        int value;
        result = getValue(attributes, "int", value);
        CHECK(result);
        CHECK(123 == value);
    }
    {
        auto result = setValue(attributes, "double", 1.23);
        CHECK(result);
        double value;
        result = getValue(attributes, "double", value);
        CHECK(result);
        CHECK(1.23 == value);
    }
    {
        auto result = setValue(attributes, "string", "xyz");
        CHECK(result);
        std::string value;
        result = getValue(attributes, "string", value);
        CHECK(result);
        CHECK("xyz" == value);

        attributes["string"] = "abc";
        value = attributes["string"];
        CHECK("abc" == value);

        using namespace xml::lite::literals;  // _q and _u for QName and Uri
        attributes["string"_q] = "123";
        value = attributes["string"_q];
        CHECK("123" == value);
    }
    {
        auto toString = [](const bool& value) { return value ? "yes" : "no"; };
        auto result = setValue(attributes, "bool", true, toString);
        CHECK(result);

        auto toType = [](const std::string& value) { return value == "yes"; };
        bool value = false;
        result = castValue(attributes, "bool", value, toType);
        CHECK(result);
        CHECK(true == value);
        std::string strValue;
        result = getValue(attributes, "bool", strValue);
        CHECK(result);
        CHECK("yes" == strValue);
    }

    {
        const auto result = setValue(attributes, "not_found", 999);
        CHECK_FALSE(result);
    }
}

TEST_CASE("test_setAttributeValueByIndex")
{
    test_MinidomParser xmlParser;
    auto root = xmlParser.getRootElement();

    auto& values = root->getElementByTagName("values", true /*recurse*/);
    auto& attributes = values.getAttributes();

    {
        auto result = setValue(attributes, 0, 123);
        CHECK(result);
        int value;
        result = getValue(attributes, 0, value);
        CHECK(result);
        CHECK(123 == value);
    }
    {
        auto result = setValue(attributes, 1, 1.23);
        CHECK(result);
        double value;
        result = getValue(attributes, 1, value);
        CHECK(result);
        CHECK(1.23 == value);
    }
    {
        auto result = setValue(attributes, 2, "xyz");
        CHECK(result);
        std::string value;
        result = getValue(attributes, 2, value);
        CHECK(result);
        CHECK("xyz" == value);
    }
    {
        auto result = setValue(attributes, -1, -1);
        CHECK_FALSE(result);
        result = setValue(attributes, 999, 999);
        CHECK_FALSE(result);
    }
}
