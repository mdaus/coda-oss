/* =========================================================================
 * This file is part of str-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * str-c++ is free software; you can redistribute it and/or modify
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

#include <tuple> // std::ignore
#include <span>

#include <types/Complex.h>
#include <config/compiler_extensions.h>
#include <import/str.h>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

TEST_CASE("testTrim")
{
    std::string s = "  test   ";
    str::trim( s);
    CHECK(s == "test");
}

TEST_CASE("testData")
{
    std::string s;
    // https://en.cppreference.com/w/cpp/string/basic_string/resize
    s.resize(3); // "Resizes the string to contain count characters." 
    
    CODA_OSS_disable_warning_push
    #if _MSC_VER
    #pragma warning(disable : 4996)  // '...': This function or variable may be unsafe. ...
    #endif

    // https://en.cppreference.com/w/cpp/string/basic_string/data
    // "Modifying the past-the-end null terminator stored at data()+size() to any value other than CharT() has undefined behavior."
    std::ignore = strcpy(str::data(s), "abc"); 
    
    CODA_OSS_disable_warning_pop
    CHECK(s == "abc");
}

TEST_CASE("testUpper")
{
    const std::string s_ = "test-something1";
    std::string s = s_;
    CHECK(str::eq(s, "TEST-SOMETHING1"));
    str::upper( s);
    CHECK(s == "TEST-SOMETHING1");

    //#if _WIN32
    //s = "<��a`�o\"�o/�b�>";
    //str::w1252_upper(s);
    //TEST_ASSERT_EQ(s, "<��A`�O\"�O/�B�>");
    //#endif
}

TEST_CASE("test_toupper")
{
    for (uint16_t i = 0x20; i <= 0xff; i++) // uint16_t to avoid wrap-around
    {
        const auto w1252 = static_cast<str::Windows1252_T>(i);
        const auto w1252_upper = str::to_w1252_upper(w1252);

        const auto w1252_lower = w1252 == w1252_upper ? w1252 : str::to_w1252_lower(w1252_upper); // round-trip
        CHECK(static_cast<uint8_t>(w1252) == static_cast<uint8_t>(w1252_lower));

        if (i <= 0x7f) // ASCII
        {
            const auto ch = static_cast<char>(i);
            const auto upper = toupper(ch);
            CHECK(static_cast<uint8_t>(upper) == static_cast<uint8_t>(w1252_upper));

            const auto lower = ch == upper ? ch : tolower(upper); // round-trip
            CHECK(ch == lower);
            CHECK(static_cast<uint8_t>(lower) == static_cast<uint8_t>(w1252_lower));
        }
    }
}

TEST_CASE("testLower")
{
    const std::string s_ = "TEST1";
    std::string s = s_;
    CHECK(str::eq(s, "test1"));
    str::lower(s);
    CHECK(s == "test1");

    //#if _WIN32
    //s = "[������]";
    //str::w1252_lower(s);
    //TEST_ASSERT_EQ(s, "[������]");
    //#endif
}

TEST_CASE("test_tolower")
{
    for (uint16_t i = 0x20; i <= 0xff; i++) // uint16_t to avoid wrap-around
    {
        const auto w1252 = static_cast<str::Windows1252_T>(i);
        const auto w1252_lower = str::to_w1252_lower(w1252);

        const auto w1252_upper = w1252 == w1252_lower ? w1252 : str::to_w1252_upper(w1252_lower); // round-trip
        CHECK(static_cast<uint8_t>(w1252) == static_cast<uint8_t>(w1252_upper));

        if (i <= 0x7f) // ASCII
        {
            const auto ch = static_cast<char>(i);
            const auto lower = tolower(ch);
            CHECK(static_cast<uint8_t>(lower) == static_cast<uint8_t>(w1252_lower));

            const auto upper = ch == lower ? ch : toupper(lower); // round-trip
            CHECK(ch == upper);
            CHECK(static_cast<uint8_t>(upper) == static_cast<uint8_t>(w1252_upper));
        }
    }
}

TEST_CASE("test_eq_ne")
{
    const auto s1 = "TEST1";
    const auto s2 = "test1";
    const auto s3 = "T2";

    CHECK(str::eq(s1, s1));
    CHECK_FALSE(str::ne(s1, s1));

    CHECK(str::eq(s1, s2));
    CHECK_FALSE(str::ne(s1, s2));
    CHECK(str::eq(s2, s1));
    CHECK_FALSE(str::ne(s2, s1));

    CHECK_FALSE(str::eq(s1, s3));
    CHECK(str::ne(s1, s3));
    CHECK_FALSE(str::eq(s3, s1));
    CHECK(str::ne(s3, s1));
}

TEST_CASE("testReplace")
{
    std::string s = "helo world";
    str::replace(s, "l", "ll");
    CHECK(s == "hello world");
}

TEST_CASE("testReplaceAllInfinite")
{
    std::string s = "helo hello";
    str::replaceAll(s, "l", "ll");
    CHECK(s == "hello hellllo");
}

TEST_CASE("testReplaceAllRecurse")
{
    std::string s = "Mississippi";
    str::replaceAll(s, "i", " ");
    CHECK(s == "M ss ss pp ");
}

TEST_CASE("testContains")
{
    std::string s = "Mississippi";
    CHECK(str::contains(s, "ssiss"));
}

TEST_CASE("testNotContains")
{
    std::string s = "Mississippi";
    CHECK_FALSE(str::contains(s, "miss"));
}

TEST_CASE("testSplit")
{
    std::string s = "space delimited values are the best!";
    std::vector<std::string> parts = str::split(s, " ");
    CHECK(std::ssize(parts) == 6);
    parts = str::split(s, " ", 3);
    CHECK(std::ssize(parts) == 3);
    CHECK(parts[2] == "values are the best!");
}

TEST_CASE("testIsAlpha")
{
    CHECK(str::isAlpha("abcdefghijklmnopqrstuvwxyz"));
    CHECK_FALSE(str::isAlpha("abc123"));
    CHECK_FALSE(str::isAlpha("abcs with spaces"));
}
TEST_CASE("testIsAlphaSpace")
{
    CHECK(str::isAlphaSpace("abcdefghijklmnopqrstuvwxyz"));
    CHECK_FALSE(str::isAlphaSpace("abc123"));
    CHECK(str::isAlphaSpace("abcs with spaces"));
}
TEST_CASE("testIsNumeric")
{
    CHECK_FALSE(str::isNumeric("abcdefghijklmnopqrstuvwxyz"));
    CHECK_FALSE(str::isNumeric("abc123"));
    CHECK_FALSE(str::isNumeric("abcs with spaces"));
    CHECK(str::isNumeric("42"));
}
TEST_CASE("testIsNumericSpace")
{
    CHECK_FALSE(str::isNumericSpace("lotto47"));
    CHECK(str::isNumericSpace("42"));
    CHECK(str::isNumericSpace("42 15 23 5 12"));
}
TEST_CASE("testIsAlphanumeric")
{
    CHECK(str::isAlphanumeric("lotto47"));
    CHECK(str::isAlphanumeric("42"));
    CHECK_FALSE(str::isAlphanumeric("42 15 23 5 12"));
    CHECK(str::isAlphanumeric("justtext"));
}
TEST_CASE("testIsWhitespace")
{
    CHECK_FALSE(str::isWhitespace("lotto47"));
    CHECK(str::isWhitespace(""));
    CHECK(str::isWhitespace(" "));
    CHECK(str::isWhitespace("                          "));
    CHECK(str::isWhitespace("\t"));
    CHECK(str::isWhitespace("\t \n"));
}
TEST_CASE("testContainsOnly")
{
    CHECK(str::containsOnly("abc", "abcdefghijklmnopqrstuvwxyz"));
    CHECK_FALSE(str::containsOnly("abc!", "abcdefghijklmnopqrstuvwxyz"));
    CHECK(str::containsOnly("some-cool-id", "-abcdefghijklmnopqrstuvwxyz"));
    CHECK(str::containsOnly("\n\r\t ", " \t\n\r0123456789"));
    CHECK(str::containsOnly("1-2-3", " \t\n\r0123456789-"));
}
TEST_CASE("testRoundDouble")
{
    double eps = std::numeric_limits<double>::epsilon();
    double numerator = 10005.0;
    double denom = 10007.0;
    double v = numerator / denom;
    std::string s = str::toString(v);
    std::cout << s << std::endl;

    double nv = str::toType<double>(s);
    CHECK_THAT(nv, Catch::Matchers::WithinRel(v, eps));
    nv *= denom;
    CHECK_THAT(nv, Catch::Matchers::WithinRel(numerator, eps));
    std::cout << nv << std::endl;
    std::cout << (nv - static_cast<int>(nv)) << std::endl;
    std::cout << std::numeric_limits<double>::epsilon() << std::endl;
    CHECK(static_cast<int>(std::ceil(nv)) == static_cast<int>(numerator));
}

TEST_CASE("testEscapeForXMLNoReplace")
{
    const std::string origMessage("This is a perfectly fine string");
    std::string message(origMessage);
    str::escapeForXML(message);
    CHECK(message == origMessage);
}

TEST_CASE("testEscapeForXMLKitchenSink")
{
    std::string message(
            "This & that with <angles> and \"quotes\" & single 'quotes' & "
            "why not a\nnewline & \rcarriage return at the end?");

    const std::string expectedMessage(
            "This &amp; that with &lt;angles&gt; and &quot;quotes&quot; &amp; "
            "single &apos;quotes&apos; &amp; why not a&#10;newline &amp; "
            "&#13;carriage return at the end?");

    str::escapeForXML(message);
    CHECK(message == expectedMessage);
}

TEST_CASE("test_toStringComplexFloat")
{
    const std::string expected("(1,-2)");

    const std::complex<float> std_cx_float(1.0f, -2.0f);
    auto actual = str::toString(std_cx_float);
    CHECK(actual == expected);

    const types::ComplexReal<float> types_cx_float(1.0f, -2.0f);
    actual = str::toString(types_cx_float);
    CHECK(actual == expected);

    const types::zfloat zfloat(1.0f, -2.0f);
    actual = str::toString(zfloat);
    CHECK(actual == expected);
}
TEST_CASE("test_toTypeComplexFloat")
{
    const std::string strValue("(1,-2)");

    auto actual = str::toType<std::complex<float>>(strValue);
    auto strActual = str::toString(actual);
    CHECK(strActual == strValue);

    actual = str::toType<types::ComplexReal<float>>(strValue);
    strActual = str::toString(actual);
    CHECK(strActual == strValue);

    actual = str::toType<types::zfloat>(strValue);
    strActual = str::toString(actual);
    CHECK(strActual == strValue);
}

TEST_CASE("test_toStringComplexShort")
{
    const std::string expected("(1,-2)");

    CODA_OSS_disable_warning_push
    #if _MSC_VER
    #pragma warning(disable: 4996) // '...': warning STL4037: The effect of instantiating the template std::complex for any type other than float, double, or long double is unspecified. You can define _SILENCE_NONFLOATING_COMPLEX_DEPRECATION_WARNING to suppress this warning
    #endif
    const std::complex<short> std_cx_short(1, -2);
    CODA_OSS_disable_warning_pop
    auto actual = str::toString(std_cx_short);
    CHECK(actual == expected);

    const types::ComplexInteger<short> types_cx_short(std_cx_short);  // "copy constructor" or overload
    actual = str::toString(types_cx_short);
    CHECK(actual == expected);

    const types::Complex<int16_t> zint16(1, -2);
    actual = str::toString(zint16);
    CHECK(actual == expected);
}
TEST_CASE("test_toTypeComplexShort")
{
    const std::string strValue("(1,-2)");

    CODA_OSS_disable_warning_push
    #if _MSC_VER
    #pragma warning(disable: 4996) // '...': warning STL4037: The effect of instantiating the template std::complex for any type other than float, double, or long double is unspecified. You can define _SILENCE_NONFLOATING_COMPLEX_DEPRECATION_WARNING to suppress this warning
    #endif
    const auto cx_actual = str::toType<std::complex<short>>(strValue);
    CODA_OSS_disable_warning_pop
    auto strActual = str::toString(cx_actual);
    CHECK(strActual == strValue);

    CODA_OSS_disable_warning_push
    #if _MSC_VER
    #pragma warning(disable: 4996) // '...': warning STL4037: The effect of instantiating the template std::complex for any type other than float, double, or long double is unspecified. You can define _SILENCE_NONFLOATING_COMPLEX_DEPRECATION_WARNING to suppress this warning
    #endif
    auto zactual = str::toType<types::ComplexInteger<short>>(strValue);
    CODA_OSS_disable_warning_pop
    strActual = str::toString(zactual);
    CHECK(strActual == strValue);

    zactual = str::toType<types::Complex<int16_t>>(strValue);
    strActual = str::toString(zactual);
    CHECK(strActual == strValue);
}
