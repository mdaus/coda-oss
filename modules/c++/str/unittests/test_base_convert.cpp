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

#include <wchar.h>

#include <vector>
#include <string>
#include <iterator>
#include <map>
#if _WIN32
#include <comdef.h>
#endif

#include <config/compiler_extensions.h>
#include <import/str.h>
#include <str/Encoding.h>
#include <gsl/gsl.h>
#include <sys/OS.h>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("testConvert")
{
    CHECK(str::toType<long long>("0x3BC7", 16) == gsl::narrow<long long>(0x3BC7));
    CHECK(str::toType<long long>("1101", 2) == gsl::narrow<long long>(13));
    CHECK(str::toType<long long>("231", 5) == gsl::narrow<long long>(66));
    CHECK(str::toType<unsigned long long>("0xFFFFFFFFFFFFFFFF", 16) == gsl::narrow<unsigned long long>(0xFFFFFFFFFFFFFFFF));
    CHECK(str::toType<unsigned long long>("-10", 10) == gsl::narrow_cast<unsigned long long>(-10));
    CHECK(str::toType<short>("13", 4) == gsl::narrow<short>(7));
}

TEST_CASE("testBadConvert")
{
    CHECK_THROWS(str::toType<long long>("Not a number", 10));
    CHECK_THROWS(str::toType<long long>("0xFFFFFFFFFFFFFFFF", 16));
    CHECK_THROWS(str::toType<short>("0xFFFFF", 16));
}

TEST_CASE("testEightBitIntToString")
{
    CHECK(std::to_string(static_cast<uint8_t>(1)) == "1");
    CHECK(std::to_string(static_cast<int8_t>(2)) == "2");
    CHECK(std::to_string(static_cast<int8_t>(-2)) == "-2");
}

TEST_CASE("testCharToString")
{
    CHECK(str::toString('a') == "a");
    CHECK(str::toString(static_cast<char>(65)) == "A");
}

template<typename T>
static constexpr std::u8string::value_type cast8(T ch)
{
    using u8ch_t = std::u8string::value_type;
    static_assert(sizeof(u8ch_t) == sizeof(char), "sizeof(Char8_T) != sizeof(char)");
    return static_cast<u8ch_t>(ch);
}

TEST_CASE("test_string_to_u8string_ascii")
{
    {
        const std::string input = "|\x00";  //  ASCII, "|<NULL>"
        const auto actual = str::to_u8string<str::W1252string>(input);
        const std::u8string expected{cast8('|')}; // '\x00' is the end of the string in C/C++
        CHECK(actual == expected);
    }
    constexpr uint8_t start_of_heading = 0x01;
    constexpr uint8_t delete_character = 0x7f;
    for (uint8_t ch = start_of_heading; ch <= delete_character; ch++)  // ASCII
    {
        const std::string input { '|', static_cast<std::string::value_type>(ch), '|'};
        const auto actual = str::to_u8string<str::W1252string>(input);
        const std::u8string expected8{cast8('|'), cast8(ch), cast8('|')}; 
        CHECK(actual == expected8);
        const std::u32string expected{U'|', ch, U'|'};
        CHECK(actual == str::to_u8string(expected));
    }
}


TEST_CASE("test_string_to_u8string_windows_1252")
{
    // Windows-1252 only characters must be mapped to UTF-8
    {
        const std::string input = "|\x80|";  // Windows-1252, "|�|"
        const auto actual = str::to_u8string<str::W1252string>(input);
        const std::u8string expected8{cast8('|'), cast8('\xE2'), cast8('\x82'), cast8('\xAC'), cast8('|')};  // UTF-8,  "|�|"
        CHECK(actual == expected8);
        const std::u32string expected{U"|\u20AC|"};  // UTF-32,  "|�|"
        CHECK(actual == str::to_u8string(expected));
    }
    {
        const std::string input = "|\x9F|";  // Windows-1252, "|�|"
        const auto actual = str::to_u8string<str::W1252string>(input);
        const std::u8string expected8{cast8('|'), cast8('\xC5'), cast8('\xB8'), cast8('|')};  // UTF-8,  "|�|"
        CHECK(actual == expected8);
        const std::u32string expected{U"|\u0178|"};  // UTF-32,  "|�|"
        CHECK(actual == str::to_u8string(expected));
    }
    {
        const std::vector<char> undefined{ '\x81', '\x8d', '\x8f', '\x90', '\x9d' };
        for (const auto& ch : undefined)
        {
            const std::string input{'|', ch, '|'};
            const auto actual = str::to_u8string<str::W1252string>(input);
            CHECK(!actual.empty());
            //const std::u8string expected8{cast8('|'), cast8('\xEF'), cast8('\xBF'), cast8('\xBD'), cast8('|')};  // UTF-8,  "|<REPLACEMENT CHARACTER>|"
            const std::u8string expected8{cast8('|'), cast8(194), cast8(ch), cast8('|')};
            CHECK(actual == expected8);
            //const std::u32string expected{U"|\ufffd|"};  // UTF-32,  "|<REPLACEMENT CHARACTER>|"
            const auto expected = str::to_u32string(expected8);
            CHECK(actual == str::to_u8string(expected));
        }    
    }
    {
        //  http://www.unicode.org/Public/MAPPINGS/VENDORS/MICSFT/WINDOWS/CP1252.TXT
        const std::vector<uint8_t> windows1252_characters{
                0x80,  // EURO SIGN
                0x82,  // SINGLE LOW-9 QUOTATION MARK
                0x83,  // LATIN SMALL LETTER F WITH HOOK
                0x84,  // DOUBLE LOW-9 QUOTATION MARK
                0x85,  // HORIZONTAL ELLIPSIS
                0x86,  // DAGGER
                0x87,  // DOUBLE DAGGER
                0x88,  // MODIFIER LETTER CIRCUMFLEX ACCENT
                0x89,  // PER MILLE SIGN
                0x8A,  // LATIN CAPITAL LETTER S WITH CARON
                0x8B,  // SINGLE LEFT-POINTING ANGLE QUOTATION MARK
                0x8C,  // LATIN CAPITAL LIGATURE OE
                0x8E,  // LATIN CAPITAL LETTER Z WITH CARON
                0x91,  // LEFT SINGLE QUOTATION MARK
                0x92,  // RIGHT SINGLE QUOTATION MARK
                0x93,  // LEFT DOUBLE QUOTATION MARK
                0x94,  // RIGHT DOUBLE QUOTATION MARK
                0x95,  // BULLET
                0x96,  // EN DASH
                0x97,  // EM DASH
                0x98,  // SMALL TILDE
                0x99,  // TRADE MARK SIGN
                0x9A,  // LATIN SMALL LETTER S WITH CARON
                0x9B,  // SINGLE RIGHT-POINTING ANGLE QUOTATION MARK
                0x9C,  // LATIN SMALL LIGATURE OE
                0x9E,  // LATIN SMALL LETTER Z WITH CARON
                0x9F};  // LATIN CAPITAL LETTER Y WITH DIAERESIS
        std::string runningInput1;
        std::string runningInput2;
        for (const auto& ch_ : windows1252_characters)
        {
            const auto ch = static_cast<std::string::value_type>(ch_);
            const std::string input_{'[', ch, ']'};
            {
                const auto input(str::str<str::W1252string>(input_));
                const auto actual = str::to_u8string(input);

                // No "expected" to test against as the UTF-8 values for these Windows-1252 characters
                // are mapped one-by-one.  However, we can test that UTF-8 to Windows-1252
                // works as that walks through a UTF-8 string which can have 1-, 2-, 3- and 4-bytes
                // for a single code-point.
                const auto w1252 = str::to_w1252string(actual.data(), actual.size());
                CHECK(input == w1252);
            }

            runningInput1 += ch;
            {
                const auto input(str::str<str::W1252string>(runningInput1));
                const auto actual = str::to_u8string(input);

                // No "expected" to test against as the UTF-8 values for these Windows-1252 characters
                // are mapped one-by-one.  However, we can test that UTF-8 to Windows-1252
                // works as that walks through a UTF-8 string which can have 1-, 2-, 3- and 4-bytes
                // for a single code-point.
                const auto w1252 = str::to_w1252string(actual.data(), actual.size());
                CHECK(input == w1252);
            }

            runningInput2 += input_;
            {
                const auto input(str::str<str::W1252string>(runningInput2));
                const auto actual = str::to_u8string(input);

                // No "expected" to test against as the UTF-8 values for these Windows-1252 characters
                // are mapped one-by-one.  However, we can test that UTF-8 to Windows-1252
                // works as that walks through a UTF-8 string which can have 1-, 2-, 3- and 4-bytes
                // for a single code-point.
                const auto w1252 = str::to_w1252string(actual.data(), actual.size());
                CHECK(input == w1252);
            }
            {
                const auto input(str::str<str::W1252string>(runningInput1+runningInput2));
                const auto actual = str::to_u8string(input);

                // No "expected" to test against as the UTF-8 values for these Windows-1252 characters
                // are mapped one-by-one.  However, we can test that UTF-8 to Windows-1252
                // works as that walks through a UTF-8 string which can have 1-, 2-, 3- and 4-bytes
                // for a single code-point.
                const auto w1252 = str::to_w1252string(actual.data(), actual.size());
                CHECK(input == w1252);
            }
            {
                const auto input(str::str<str::W1252string>(runningInput2+runningInput1));
                const auto actual = str::to_u8string(input);

                // No "expected" to test against as the UTF-8 values for these Windows-1252 characters
                // are mapped one-by-one.  However, we can test that UTF-8 to Windows-1252
                // works as that walks through a UTF-8 string which can have 1-, 2-, 3- and 4-bytes
                // for a single code-point.
                const auto w1252 = str::to_w1252string(actual.data(), actual.size());
                CHECK(input == w1252);
            }
        }    
    }
}

TEST_CASE("test_string_to_u8string_iso8859_1")
{
    constexpr uint8_t nobreak_space = 0xa0;
    constexpr uint8_t latin_small_letter_y_with_diaeresis = 0xff;  // '�'
    for (uint32_t ch = nobreak_space; ch <= latin_small_letter_y_with_diaeresis; ch++)  // ISO8859-1
    {
        const std::string input_ { '|', static_cast<std::string::value_type>(ch), '|'};
        const auto input(str::str<str::W1252string>(input_));
        const auto actual = to_u8string(input);
        const std::u32string expected{U'|', ch, U'|'};
        CHECK(actual == str::to_u8string(expected));

        CHECK(str::to_u8string(input) == actual);
        CHECK(input == str::to_w1252string(actual));
    }
}

TEST_CASE("test_change_case")
{
    const std::string ABC = "ABC";
    const std::string abc = "abc";
    {
        const auto lower = abc;
        const auto upper = ABC;
        std::string s = str::lower(upper);
        CHECK(s == lower);
        s = str::upper(lower);
        CHECK(s == upper);

        s = str::upper(upper);
        CHECK(s == upper);
        s = str::lower(lower);
        CHECK(s == lower);
    }

    // const std::wstring ABC_w = L"ABC";
    // const std::wstring abc_w = L"abc";
    // {
    //     const auto lower = abc_w;
    //     const auto upper = ABC_w;
    //     std::wstring s = str::lower(upper);
    //     CHECK(s == lower);
    //     s = str::upper(lower);
    //     CHECK(s == upper);

    //     s = str::upper(upper);
    //     CHECK(s == upper);
    //     s = str::lower(lower);
    //     CHECK(s == lower);
    // }

    // Yes, this can really come up, "non classifi�" is French (Canadian) for "unclassified".
    const std::string DEF_1252_{'D', '\xc9', 'F'}; // "D�F" Windows-1252
    const auto DEF_1252 = str::str<str::W1252string>(DEF_1252_);
    const auto DEF8 = str::to_u8string(DEF_1252);

    const std::string def_1252_{'d', '\xe9', 'f'};  // "d�f" Windows-1252
    const auto def_1252 = str::str<str::W1252string>(def_1252_);
    const auto def8 = str::to_u8string(def_1252);

    // {
    //     const auto lower = def8;
    //     const auto upper = DEF8;
    //     std::u8string s = str::lower(upper);
    //     CHECK(s == lower);
    //     s = str::upper(lower);
    //     CHECK(s == upper);

    //     s = str::upper(upper);
    //     CHECK(s == upper);
    //     s = str::lower(lower);
    //     CHECK(s == lower);
    // }
    // {
    //     const auto lower = def_1252;
    //     const auto upper = DEF_1252;
    //     str::W1252string s = str::lower(upper);
    //     CHECK(s == lower);
    //     s = str::upper(lower);
    //     CHECK(s == upper);

    //     s = str::upper(upper);
    //     CHECK(s == upper);
    //     s = str::lower(lower);
    //     CHECK(s == lower);
    // }
}

// https://en.wikipedia.org/wiki/%C3%89#Character_mappings
static const std::u8string& classificationText_u8()
{
    static const auto retval(str::make_string<std::u8string>("A\xc3\x89IOU")); // UTF-8 "A�IOU"
    return retval;
 }

static const str::W1252string& classificationText_w1252()
 {
    static const auto retval(str::make_string<str::W1252string>("A\xc9IOU"));  // ISO8859-1 "A�IOU"    
    return retval;
 }

 static auto toString(const std::u8string& s)
 {
     return str::to_native(s);
 }

// UTF-16 on Windows, UTF-32 on Linux
static const wchar_t* classificationText_wide_() { return L"A\x00c9IOU"; } // "wide characters" "A�IOU"
static std::u16string classificationText_u16() { return u"A\u00c9IOU"; } // UTF-16 "A�IOU"
static std::u32string classificationText_u32() { return U"A\u00c9IOU"; } // UTF-32 "A�IOU"

static std::string classificationText_platform() { return 
    sys::Platform == sys::PlatformType::Linux ? toString(classificationText_u8()) : str::testing::to_string(classificationText_w1252()); }

TEST_CASE("test_u8string_to_string")
{
    auto actual = toString(classificationText_u8());
    CHECK(classificationText_platform() == actual);

    actual = str::testing::to_string(classificationText_w1252()); 
    CHECK(classificationText_platform() == actual);
}

static auto w1252FromNative(const std::wstring& s)
{
    return str::to_w1252string(str::u8FromNative(s));
}
static auto toWString(const str::W1252string& s)
{
    return str::testing::to_wstring(s);
}
static auto toWString(const std::u8string& s)
{
    return str::details::to_wstring(s);
}

TEST_CASE("test_u8string_to_u16string")
{
    const auto actual = classificationText_u16();
    const std::wstring wide(classificationText_wide_());
    #if _WIN32
    const auto s = str::str<std::wstring>(actual); // Windows: std::wstring == std::u16string
    CHECK(wide == s);  // _EQ wants to do toString()
    #endif

    const auto u8 = classificationText_u8();
    CHECK(str::u8FromNative(wide) == u8);
    CHECK(wide == toWString(u8));
    
    const auto w1252 = str::c_str<str::W1252string>(classificationText_w1252());
    CHECK(w1252FromNative(wide) == w1252);
    CHECK(wide == toWString(w1252));

    CHECK(classificationText_u16() == actual);  // _EQ wants to do toString()
    CHECK(classificationText_u16() == to_u16string(w1252)); // _EQ wants to do toString()
}

TEST_CASE("test_u8string_to_u32string")
{
    const auto actual = classificationText_u32();
#if !_WIN32
    const auto s  = str::str<std::wstring>(actual); // Linux: std::wstring == std::u32string
    CHECK(classificationText_wide_() == s); // _EQ wants to do toString()
#endif
    
    const std::wstring wide(classificationText_wide_());
    const auto u8 = classificationText_u8();
    CHECK(str::u8FromNative(wide) == u8);
    CHECK(wide == toWString(u8));

    const auto w1252 = str::c_str<str::W1252string>(classificationText_w1252());
    CHECK(w1252FromNative(wide) == w1252);
    CHECK(wide == toWString(w1252));

    CHECK(classificationText_u32() == actual);  // _EQ wants to do toString()
    CHECK(classificationText_u32() == to_u32string(w1252)); // _EQ wants to do toString()
}

static auto toWString(const std::u16string& s)
{
    return toWString(str::to_u8string(s));
}
static auto toWString(const std::string& s)
{
    return str::details::to_wstring(s);
}
static auto toString(const std::u16string& s)
{
    return str::details::to_string(str::to_u8string(s));
}

TEST_CASE("test_ASCII_0")
{
   // https://en.cppreference.com/w/cpp/language/escape
    constexpr auto escapes = "|\'|\"|\?|\\|\a|\b|\f|\n|\r|\t|\v|";
    constexpr auto u16_escapes = u"|\'|\"|\?|\\|\a|\b|\f|\n|\r|\t|\v|";
    const char* LpStr = escapes;
    std::u16string::const_pointer LpUtf16 = u16_escapes;
    // For both UTF-8 and Windows-1252, ASCII is the same (they only differ for 0x80-0xff).
    const auto u8 =  str::str<std::string>(str::to_u8string<std::u8string>(LpStr));
    CHECK(LpStr == u8); // native() is the same on all platforms/encodings for ASCII
    {
        const auto w1252 = str::make_string<str::W1252string>(LpStr);
        const auto str1252 = str::testing::to_string(w1252);
        CHECK(LpStr == str1252);  // native() is the same on all platforms/encodings for ASCII
    }

    const auto u16 = str::to_u16string(str::u8FromNative(LpStr));
    CHECK(u16 == LpUtf16);
    auto wstring = toWString(LpStr);
    std::string native = LpStr;
    auto w1252 = str::make_string<str::W1252string>(LpStr);
    {
        const char* lpStr = LpStr;
        const str::W1252string lw1252 = w1252;
        // from UTF-16 back to Windows-1252
        const auto str_w1252 = str::str<std::string>(lw1252);
        CHECK(str_w1252 == lpStr);

        #if _WIN32
            // Since we're using UTF-16, on Windows that can be cast to wchar_t
            std::u16string::const_pointer lpUtf16 = LpUtf16;
            const std::wstring lwstring = wstring;
            const std::string lnative = native;
            const auto wide = str::make_string<std::wstring>(lpUtf16);

            const _bstr_t str(lpStr);
            const std::wstring std_wstr(static_cast<const wchar_t*>(str)); // Windows-1252 -> UTF-16
            CHECK(lwstring == std_wstr);
            CHECK(std_wstr == wide);

            const _bstr_t wide_str(wide.c_str());
            const std::string std_str(static_cast<const char*>(wide_str)); //  UTF-16 -> Windows-1252
            CHECK(lnative == std_str);
            CHECK(std_str == lpStr);
        #endif
    }

    native = toString(LpUtf16);
    CHECK(native == LpStr); // native() is the same on all platforms/encodings for ASCII
    wstring = toWString(LpUtf16);
    {
        const char* lpStr = LpStr;
        const str::W1252string lw1252 = w1252;
        // from UTF-16 back to Windows-1252
        const auto str_w1252 = str::str<std::string>(lw1252);
        CHECK(str_w1252 == lpStr);

        #if _WIN32
            // Since we're using UTF-16, on Windows that can be cast to wchar_t
            std::u16string::const_pointer lpUtf16 = LpUtf16;
            const std::wstring lwstring = wstring;
            const std::string lnative = native;
            const auto wide = str::make_string<std::wstring>(lpUtf16);

            const _bstr_t str(lpStr);
            const std::wstring std_wstr(static_cast<const wchar_t*>(str)); // Windows-1252 -> UTF-16
            CHECK(lwstring == std_wstr);
            CHECK(std_wstr == wide);

            const _bstr_t wide_str(wide.c_str());
            const std::string std_str(static_cast<const char*>(wide_str)); //  UTF-16 -> Windows-1252
            CHECK(lnative == std_str);
            CHECK(std_str == lpStr);
        #endif
    }
}
TEST_CASE("test_ASCII_1")
{
    // https://en.cppreference.com/w/cpp/language/escape
    constexpr auto controls = "|\x01|\x02|\x03|\x04|\x05|\x06|\x07|\x08|\x09|\x0a|\x0b|\x0c|\x0d|\x0e|\x0f"
                              "|\x10|\x11|\x12|\x13|\x14|\x15|\x16|\x17|\x18|\x19|\x1a|\x1b|\x1c|\x1d|\x1e|\x1f";
    constexpr auto u16_controls = u"|\x01|\x02|\x03|\x04|\x05|\x06|\x07|\x08|\x09|\x0a|\x0b|\x0c|\x0d|\x0e|\x0f"
                                  u"|\x10|\x11|\x12|\x13|\x14|\x15|\x16|\x17|\x18|\x19|\x1a|\x1b|\x1c|\x1d|\x1e|\x1f";
    const char* LpStr = controls;
    std::u16string::const_pointer LpUtf16 = u16_controls;
    // For both UTF-8 and Windows-1252, ASCII is the same (they only differ for 0x80-0xff).
    const auto u8 =  str::str<std::string>(str::to_u8string<std::u8string>(LpStr));
    CHECK(LpStr == u8); // native() is the same on all platforms/encodings for ASCII
    {
        const auto w1252 = str::make_string<str::W1252string>(LpStr);
        const auto str1252 = str::testing::to_string(w1252);
        CHECK(LpStr == str1252);  // native() is the same on all platforms/encodings for ASCII
    }

    const auto u16 = str::to_u16string(str::u8FromNative(LpStr));
    CHECK(u16 == LpUtf16);
    auto wstring = toWString(LpStr);
    std::string native = LpStr;
    auto w1252 = str::make_string<str::W1252string>(LpStr);
    {
        const char* lpStr = LpStr;
        const str::W1252string lw1252 = w1252;
        // from UTF-16 back to Windows-1252
        const auto str_w1252 = str::str<std::string>(lw1252);
        CHECK(str_w1252 == lpStr);

        #if _WIN32
            // Since we're using UTF-16, on Windows that can be cast to wchar_t
            std::u16string::const_pointer lpUtf16 = LpUtf16;
            const std::wstring lwstring = wstring;
            const std::string lnative = native;
            const auto wide = str::make_string<std::wstring>(lpUtf16);

            const _bstr_t str(lpStr);
            const std::wstring std_wstr(static_cast<const wchar_t*>(str)); // Windows-1252 -> UTF-16
            CHECK(lwstring == std_wstr);
            CHECK(std_wstr == wide);

            const _bstr_t wide_str(wide.c_str());
            const std::string std_str(static_cast<const char*>(wide_str)); //  UTF-16 -> Windows-1252
            CHECK(lnative == std_str);
            CHECK(std_str == lpStr);
        #endif
    }

    native = toString(LpUtf16);
    CHECK(native == LpStr); // native() is the same on all platforms/encodings for ASCII
    wstring = toWString(LpUtf16);
    {
        const char* lpStr = LpStr;
        const str::W1252string lw1252 = w1252;
        // from UTF-16 back to Windows-1252
        const auto str_w1252 = str::str<std::string>(lw1252);
        CHECK(str_w1252 == lpStr);

        #if _WIN32
            // Since we're using UTF-16, on Windows that can be cast to wchar_t
            std::u16string::const_pointer lpUtf16 = LpUtf16;
            const std::wstring lwstring = wstring;
            const std::string lnative = native;
            const auto wide = str::make_string<std::wstring>(lpUtf16);

            const _bstr_t str(lpStr);
            const std::wstring std_wstr(static_cast<const wchar_t*>(str)); // Windows-1252 -> UTF-16
            CHECK(lwstring == std_wstr);
            CHECK(std_wstr == wide);

            const _bstr_t wide_str(wide.c_str());
            const std::string std_str(static_cast<const char*>(wide_str)); //  UTF-16 -> Windows-1252
            CHECK(lnative == std_str);
            CHECK(std_str == lpStr);
        #endif
    }
}
TEST_CASE("test_ASCII_2")
{
    // https://en.cppreference.com/w/cpp/language/ascii
    constexpr auto ascii = " !\"#0@AZaz~\x7f";
    constexpr auto u16_ascii = u" !\"#0@AZaz~\x7f";
    const char* LpStr = ascii;
    std::u16string::const_pointer LpUtf16 = u16_ascii;
    // For both UTF-8 and Windows-1252, ASCII is the same (they only differ for 0x80-0xff).
    const auto u8 =  str::str<std::string>(str::to_u8string<std::u8string>(LpStr));
    CHECK(LpStr == u8); // native() is the same on all platforms/encodings for ASCII
    {
        const auto w1252 = str::make_string<str::W1252string>(LpStr);
        const auto str1252 = str::testing::to_string(w1252);
        CHECK(LpStr == str1252);  // native() is the same on all platforms/encodings for ASCII
    }

    const auto u16 = str::to_u16string(str::u8FromNative(LpStr));
    CHECK(u16 == LpUtf16);
    auto wstring = toWString(LpStr);
    std::string native = LpStr;
    auto w1252 = str::make_string<str::W1252string>(LpStr);
    {
        const char* lpStr = LpStr;
        const str::W1252string lw1252 = w1252;
        // from UTF-16 back to Windows-1252
        const auto str_w1252 = str::str<std::string>(lw1252);
        CHECK(str_w1252 == lpStr);

        #if _WIN32
            // Since we're using UTF-16, on Windows that can be cast to wchar_t
            std::u16string::const_pointer lpUtf16 = LpUtf16;
            const std::wstring lwstring = wstring;
            const std::string lnative = native;
            const auto wide = str::make_string<std::wstring>(lpUtf16);

            const _bstr_t str(lpStr);
            const std::wstring std_wstr(static_cast<const wchar_t*>(str)); // Windows-1252 -> UTF-16
            CHECK(lwstring == std_wstr);
            CHECK(std_wstr == wide);

            const _bstr_t wide_str(wide.c_str());
            const std::string std_str(static_cast<const char*>(wide_str)); //  UTF-16 -> Windows-1252
            CHECK(lnative == std_str);
            CHECK(std_str == lpStr);
        #endif
    }

    native = toString(LpUtf16);
    CHECK(native == LpStr); // native() is the same on all platforms/encodings for ASCII
    wstring = toWString(LpUtf16);
    {
        const char* lpStr = LpStr;
        const str::W1252string lw1252 = w1252;
        // from UTF-16 back to Windows-1252
        const auto str_w1252 = str::str<std::string>(lw1252);
        CHECK(str_w1252 == lpStr);

        #if _WIN32
            // Since we're using UTF-16, on Windows that can be cast to wchar_t
            std::u16string::const_pointer lpUtf16 = LpUtf16;
            const std::wstring lwstring = wstring;
            const std::string lnative = native;
            const auto wide = str::make_string<std::wstring>(lpUtf16);

            const _bstr_t str(lpStr);
            const std::wstring std_wstr(static_cast<const wchar_t*>(str)); // Windows-1252 -> UTF-16
            CHECK(lwstring == std_wstr);
            CHECK(std_wstr == wide);

            const _bstr_t wide_str(wide.c_str());
            const std::string std_str(static_cast<const char*>(wide_str)); //  UTF-16 -> Windows-1252
            CHECK(lnative == std_str);
            CHECK(std_str == lpStr);
        #endif
    }
}


static auto to_w1252string(const std::u16string& s)
{
    return str::to_w1252string(str::to_u8string(s));
}
inline static auto toString(const std::wstring& s)
{
    return str::details::to_string(s);
}

// https://en.wikipedia.org/wiki/Windows-1252
TEST_CASE("test_Windows1252_0")
{
#if _WIN32
    // can convert with bit-twiddling
    constexpr auto w1252_a1_ff = "\xa1\xa2\xfe\xff"; 
    constexpr auto u16_w1252_a1_ff = u"\u00a1\u00a2\u00fe\u00ff";
    {
        const char* LpStr = w1252_a1_ff;
        std::u16string::const_pointer LpUtf16 = u16_w1252_a1_ff;
        const auto u16 = str::to_u16string(str::to_u8string<str::W1252string>(LpStr));
        CHECK(u16 == LpUtf16);
        auto wstring = toWString(str::to_u8string<str::W1252string>(LpStr));
        auto s = toString(str::to_u8string<str::W1252string>(LpStr));
        auto w1252 = str::make_string<str::W1252string>(LpStr);
        {
            const char* lpStr = LpStr;
            std::u16string::const_pointer lpUtf16 = LpUtf16;
            const std::wstring lwstring = wstring;
            const std::string lnative = s;
            const str::W1252string lw1252 = w1252;
            // from UTF-16 back to Windows-1252
            const auto str_w1252 = str::str<std::string>(lw1252);
            CHECK(str_w1252 == lpStr);

            #if _WIN32
                // Since we're using UTF-16, on Windows that can be cast to wchar_t
                const auto wide = str::make_string<std::wstring>(lpUtf16);

                const _bstr_t str(lpStr);
                const std::wstring std_wstr(static_cast<const wchar_t*>(str)); // Windows-1252 -> UTF-16
                CHECK(lwstring == std_wstr);
                CHECK(std_wstr == wide);

                const _bstr_t wide_str(wide.c_str());
                const std::string std_str(static_cast<const char*>(wide_str)); //  UTF-16 -> Windows-1252
                CHECK(lnative == std_str);
                CHECK(std_str == lpStr);
            #endif
        }

        wstring = toWString(LpUtf16);
        s = toString(LpUtf16);
        w1252 = to_w1252string(LpUtf16);
        {
            const char* lpStr = LpStr;
            std::u16string::const_pointer lpUtf16 = LpUtf16;
            const std::wstring lwstring = wstring;
            const std::string lnative = s;
            const str::W1252string lw1252 = w1252;
            // from UTF-16 back to Windows-1252
            const auto str_w1252 = str::str<std::string>(lw1252);
            CHECK(str_w1252 == lpStr);

            #if _WIN32
                // Since we're using UTF-16, on Windows that can be cast to wchar_t
                const auto wide = str::make_string<std::wstring>(lpUtf16);

                const _bstr_t str(lpStr);
                const std::wstring std_wstr(static_cast<const wchar_t*>(str)); // Windows-1252 -> UTF-16
                CHECK(lwstring == std_wstr);
                CHECK(std_wstr == wide);

                const _bstr_t wide_str(wide.c_str());
                const std::string std_str(static_cast<const char*>(wide_str)); //  UTF-16 -> Windows-1252
                CHECK(lnative == std_str);
                CHECK(std_str == lpStr);
            #endif
        }
    }
#else
    SKIP("Only runs on Windows");
#endif
}
TEST_CASE("test_Windows1252_1")
{
#if _WIN32
    constexpr auto w1252 = "\x80\x82\x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8e\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9e\x9f";
    constexpr auto u16_utf8 = u"\u20ac\u201a\u0192\u201e\u2026\u2020\u2021\u02c6\u2030\u0160\u2039\u0152\u017d"
        "\u2018\u2019\u201c\u201d\u2022\u2013\u2014\u02dc\u2122\u0161\u203a\u0153\u017e\u0178";
    {
        const char* LpStr = w1252;
        std::u16string::const_pointer LpUtf16 = u16_utf8;
        const auto u16 = str::to_u16string(str::to_u8string<str::W1252string>(LpStr));
        CHECK(u16 == LpUtf16);
        auto wstring = toWString(str::to_u8string<str::W1252string>(LpStr));
        auto s = toString(str::to_u8string<str::W1252string>(LpStr));
        auto w1252 = str::make_string<str::W1252string>(LpStr);
        {
            const char* lpStr = LpStr;
            std::u16string::const_pointer lpUtf16 = LpUtf16;
            const std::wstring lwstring = wstring;
            const std::string lnative = s;
            const str::W1252string lw1252 = w1252;
            // from UTF-16 back to Windows-1252
            const auto str_w1252 = str::str<std::string>(lw1252);
            CHECK(str_w1252 == lpStr);

            #if _WIN32
                // Since we're using UTF-16, on Windows that can be cast to wchar_t
                const auto wide = str::make_string<std::wstring>(lpUtf16);

                const _bstr_t str(lpStr);
                const std::wstring std_wstr(static_cast<const wchar_t*>(str)); // Windows-1252 -> UTF-16
                CHECK(lwstring == std_wstr);
                CHECK(std_wstr == wide);

                const _bstr_t wide_str(wide.c_str());
                const std::string std_str(static_cast<const char*>(wide_str)); //  UTF-16 -> Windows-1252
                CHECK(lnative == std_str);
                CHECK(std_str == lpStr);
            #endif
        }

        wstring = toWString(LpUtf16);
        s = toString(LpUtf16);
        w1252 = to_w1252string(LpUtf16);
        {
            const char* lpStr = LpStr;
            std::u16string::const_pointer lpUtf16 = LpUtf16;
            const std::wstring lwstring = wstring;
            const std::string lnative = s;
            const str::W1252string lw1252 = w1252;
            // from UTF-16 back to Windows-1252
            const auto str_w1252 = str::str<std::string>(lw1252);
            CHECK(str_w1252 == lpStr);

            #if _WIN32
                // Since we're using UTF-16, on Windows that can be cast to wchar_t
                const auto wide = str::make_string<std::wstring>(lpUtf16);

                const _bstr_t str(lpStr);
                const std::wstring std_wstr(static_cast<const wchar_t*>(str)); // Windows-1252 -> UTF-16
                CHECK(lwstring == std_wstr);
                CHECK(std_wstr == wide);

                const _bstr_t wide_str(wide.c_str());
                const std::string std_str(static_cast<const char*>(wide_str)); //  UTF-16 -> Windows-1252
                CHECK(lnative == std_str);
                CHECK(std_str == lpStr);
            #endif
        }
    }
#else
    SKIP("Only runs on Windows");
#endif
}
TEST_CASE("test_Windows1252_2")
{
#if _WIN32
    // This only works with "relaxed" (i.e., not "strict") conversion; which is what _bstr_t does
    constexpr auto w1252_unassigned = "\x81\x8d\x8f\x90\x9d";
    constexpr auto u16_w1252_unassigned = u"\x81\x8d\x8f\x90\x9d";
    {
        const char* LpStr = w1252_unassigned;
        std::u16string::const_pointer LpUtf16 = u16_w1252_unassigned;
        const auto u16 = str::to_u16string(str::to_u8string<str::W1252string>(LpStr));
        CHECK(u16 == LpUtf16);
        auto wstring = toWString(str::to_u8string<str::W1252string>(LpStr));
        auto s = toString(str::to_u8string<str::W1252string>(LpStr));
        auto w1252 = str::make_string<str::W1252string>(LpStr);
        {
            const char* lpStr = LpStr;
            std::u16string::const_pointer lpUtf16 = LpUtf16;
            const std::wstring lwstring = wstring;
            const std::string lnative = s;
            const str::W1252string lw1252 = w1252;
            // from UTF-16 back to Windows-1252
            const auto str_w1252 = str::str<std::string>(lw1252);
            CHECK(str_w1252 == lpStr);

            #if _WIN32
                // Since we're using UTF-16, on Windows that can be cast to wchar_t
                const auto wide = str::make_string<std::wstring>(lpUtf16);

                const _bstr_t str(lpStr);
                const std::wstring std_wstr(static_cast<const wchar_t*>(str)); // Windows-1252 -> UTF-16
                CHECK(lwstring == std_wstr);
                CHECK(std_wstr == wide);

                const _bstr_t wide_str(wide.c_str());
                const std::string std_str(static_cast<const char*>(wide_str)); //  UTF-16 -> Windows-1252
                CHECK(lnative == std_str);
                CHECK(std_str == lpStr);
            #endif
        }

        wstring = toWString(LpUtf16);
        s = toString(LpUtf16);
        w1252 = to_w1252string(LpUtf16);
        {
            const char* lpStr = LpStr;
            std::u16string::const_pointer lpUtf16 = LpUtf16;
            const std::wstring lwstring = wstring;
            const std::string lnative = s;
            const str::W1252string lw1252 = w1252;
            // from UTF-16 back to Windows-1252
            const auto str_w1252 = str::str<std::string>(lw1252);
            CHECK(str_w1252 == lpStr);

            #if _WIN32
                // Since we're using UTF-16, on Windows that can be cast to wchar_t
                const auto wide = str::make_string<std::wstring>(lpUtf16);

                const _bstr_t str(lpStr);
                const std::wstring std_wstr(static_cast<const wchar_t*>(str)); // Windows-1252 -> UTF-16
                CHECK(lwstring == std_wstr);
                CHECK(std_wstr == wide);

                const _bstr_t wide_str(wide.c_str());
                const std::string std_str(static_cast<const char*>(wide_str)); //  UTF-16 -> Windows-1252
                CHECK(lnative == std_str);
                CHECK(std_str == lpStr);
            #endif
        }
    }
#else
    SKIP("Only runs on Windows");
#endif
}

TEST_CASE("test_Windows1252")
{
    // https://en.wikipedia.org/wiki/Windows-1252
    const std::map<std::string::value_type, std::u16string::value_type>
            w1252_to_utf16{
          {'\x80', u'\u20AC'} // EURO SIGN
        , {'\x81', u'\u0081'} // UNDEFINED
        , {'\x82', u'\u201A'} // SINGLE LOW-9 QUOTATION MARK
        , {'\x83', u'\u0192'} // LATIN SMALL LETTER F WITH HOOK
        , {'\x84', u'\u201E'} // DOUBLE LOW-9 QUOTATION MARK
        , {'\x85', u'\u2026'} // HORIZONTAL ELLIPSIS
        , {'\x86', u'\u2020'} // DAGGER
        , {'\x87', u'\u2021'} // DOUBLE DAGGER
        , {'\x88', u'\u02C6'} // MODIFIER LETTER CIRCUMFLEX ACCENT
        , {'\x89', u'\u2030'} // PER MILLE SIGN
        , {'\x8A', u'\u0160'} // LATIN CAPITAL LETTER S WITH CARON
        , {'\x8B', u'\u2039'} // SINGLE LEFT-POINTING ANGLE QUOTATION MARK
        , {'\x8C', u'\u0152'} // LATIN CAPITAL LIGATURE OE
        , {'\x81', u'\u0081'} // UNDEFINED
        , {'\x8E', u'\u017D'} // LATIN CAPITAL LETTER Z WITH CARON
        , {'\x8F', u'\u008F'} // UNDEFINED
        , {'\x90', u'\u0090'} // UNDEFINED
        , {'\x91', u'\u2018'} // LEFT SINGLE QUOTATION MARK
        , {'\x92', u'\u2019'} // RIGHT SINGLE QUOTATION MARK
        , {'\x93', u'\u201C'} // LEFT DOUBLE QUOTATION MARK
        , {'\x94', u'\u201D'} // RIGHT DOUBLE QUOTATION MARK
        , {'\x95', u'\u2022'} // BULLET
        , {'\x96', u'\u2013'} // EN DASH
        , {'\x97', u'\u2014'} // EM DASH
        , {'\x98', u'\u02DC'} // SMALL TILDE
        , {'\x99', u'\u2122'} // TRADE MARK SIGN
        , {'\x9A', u'\u0161'} // LATIN SMALL LETTER S WITH CARON
        , {'\x9B', u'\u203A'} // SINGLE RIGHT-POINTING ANGLE QUOTATION MARK
        , {'\x9C', u'\u0153'} // LATIN SMALL LIGATURE OE
        , {'\x9D', u'\u009D'} // UNDEFINED
        , {'\x9E', u'\u017E'} // LATIN SMALL LETTER Z WITH CARON
        , {'\x9F', u'\u0178'} // LATIN CAPITAL LETTER Y WITH DIAERESIS

        , {'\xA0', u'\u00A0'} // NO-BREAK SPACE
        , {'\xA1', u'\u00A1'} // INVERTED EXCLAMATION MARK
        , {'\xA2', u'\u00A2'} // CENT SIGN
          // ...
        , {'\xFE', u'\u00FE'} // LATIN SMALL LETTER THORN
        , {'\xFF', u'\u00FF'} // LATIN SMALL LETTER Y WITH DIAERESIS
    };
    std::string running_w1252;
    std::u16string running_utf16;
    for (auto&& ch : w1252_to_utf16)
    {
        CHECK(static_cast<int>(ch.first) <= 0xff);
        CHECK(static_cast<int>(ch.second) <= 0xffff);

        const std::string w1252{ch.first};
        const std::u16string utf16{ch.second};
        {
            const char* LpStr = w1252.c_str();
            std::u16string::const_pointer LpUtf16 = utf16.c_str();
            const auto u16 = str::to_u16string(str::to_u8string<str::W1252string>(LpStr));
            CHECK(u16 == LpUtf16);
            auto wstring = toWString(str::to_u8string<str::W1252string>(LpStr));
            auto s = toString(str::to_u8string<str::W1252string>(LpStr));
            auto Lw1252 = str::make_string<str::W1252string>(LpStr);
            {
                const char* lpStr = LpStr;
                // from UTF-16 back to Windows-1252
                const auto str_w1252 = str::str<std::string>(Lw1252);
                CHECK(str_w1252 == lpStr);

                #if _WIN32
                    // Since we're using U
                    std::u16string::const_pointer lpUtf16 = LpUtf16;
                    const std::wstring lwstring = wstring;
                    const std::string lnative = s; // TF-16, on Windows that can be cast to wchar_t
                    const auto wide = str::make_string<std::wstring>(lpUtf16);

                    const _bstr_t str(lpStr);
                    const std::wstring std_wstr(static_cast<const wchar_t*>(str)); // Windows-1252 -> UTF-16
                    CHECK(lwstring == std_wstr);
                    CHECK(std_wstr == wide);

                    const _bstr_t wide_str(wide.c_str());
                    const std::string std_str(static_cast<const char*>(wide_str)); //  UTF-16 -> Windows-1252
                    CHECK(lnative == std_str);
                    CHECK(std_str == lpStr);
                #endif
            }

            wstring = toWString(LpUtf16);
            s = toString(LpUtf16);
            Lw1252 = to_w1252string(LpUtf16);
            {
                const char* lpStr = LpStr;
                // from UTF-16 back to Windows-1252
                const auto str_w1252 = str::str<std::string>(Lw1252);
                CHECK(str_w1252 == lpStr);

                #if _WIN32
                    // Since we're using U
                        std::u16string::const_pointer lpUtf16 = LpUtf16;
                    const std::wstring lwstring = wstring;
                    const std::string lnative = s; // TF-16, on Windows that can be cast to wchar_t
                    const auto wide = str::make_string<std::wstring>(lpUtf16);

                    const _bstr_t str(lpStr);
                    const std::wstring std_wstr(static_cast<const wchar_t*>(str)); // Windows-1252 -> UTF-16
                    CHECK(lwstring == std_wstr);
                    CHECK(std_wstr == wide);

                    const _bstr_t wide_str(wide.c_str());
                    const std::string std_str(static_cast<const char*>(wide_str)); //  UTF-16 -> Windows-1252
                    CHECK(lnative == std_str);
                    CHECK(std_str == lpStr);
                #endif
            }
        }

        running_w1252 += w1252;
        running_utf16 += utf16;
        {
            const char* LpStr = running_w1252.c_str();
            std::u16string::const_pointer LpUtf16 = running_utf16.c_str();
            const auto u16 = str::to_u16string(str::to_u8string<str::W1252string>(LpStr));
            CHECK(u16 == LpUtf16);
            auto wstring = toWString(str::to_u8string<str::W1252string>(LpStr));
            auto s = toString(str::to_u8string<str::W1252string>(LpStr));
            auto Lw1252 = str::make_string<str::W1252string>(LpStr);
            {
                const char* lpStr = LpStr;
                const str::W1252string lw1252 = Lw1252;
                // from UTF-16 back to Windows-1252
                const auto str_w1252 = str::str<std::string>(lw1252);
                CHECK(str_w1252 == lpStr);

                #if _WIN32
                    std::u16string::const_pointer lpUtf16 = LpUtf16;
                    const std::wstring lwstring = wstring;
                    const std::string lnative = s;
                    // Since we're using UTF-16, on Windows that can be cast to wchar_t
                    const auto wide = str::make_string<std::wstring>(lpUtf16);

                    const _bstr_t str(lpStr);
                    const std::wstring std_wstr(static_cast<const wchar_t*>(str)); // Windows-1252 -> UTF-16
                    CHECK(lwstring == std_wstr);
                    CHECK(std_wstr == wide);

                    const _bstr_t wide_str(wide.c_str());
                    const std::string std_str(static_cast<const char*>(wide_str)); //  UTF-16 -> Windows-1252
                    CHECK(lnative == std_str);
                    CHECK(std_str == lpStr);
                #endif
            }

            wstring = toWString(LpUtf16);
            s = toString(LpUtf16);
            Lw1252 = to_w1252string(LpUtf16);
            {
                const char* lpStr = LpStr;
                const str::W1252string lw1252 = Lw1252;
                // from UTF-16 back to Windows-1252
                const auto str_w1252 = str::str<std::string>(lw1252);
                CHECK(str_w1252 == lpStr);

                #if _WIN32
                    std::u16string::const_pointer lpUtf16 = LpUtf16;
                    const std::wstring lwstring = wstring;
                    const std::string lnative = s;
                    // Since we're using UTF-16, on Windows that can be cast to wchar_t
                    const auto wide = str::make_string<std::wstring>(lpUtf16);

                    const _bstr_t str(lpStr);
                    const std::wstring std_wstr(static_cast<const wchar_t*>(str)); // Windows-1252 -> UTF-16
                    CHECK(lwstring == std_wstr);
                    CHECK(std_wstr == wide);

                    const _bstr_t wide_str(wide.c_str());
                    const std::string std_str(static_cast<const char*>(wide_str)); //  UTF-16 -> Windows-1252
                    CHECK(lnative == std_str);
                    CHECK(std_str == lpStr);
                #endif
            }
        }
    }

    #if _WIN32
    // This only works on Windows because the "assume encoding" APIs are used.
    for (auto&& ch : w1252_to_utf16)
    {
        const std::string expected(1, ch.first);
        const std::wstring input(1, ch.second); // `std::wstring` is UTF-16 on Windows
        const auto actual = toString(input);
        CHECK(expected == actual);
    }
    #endif
}

TEST_CASE("test_Encoding")
{
    const auto utf_8 = toString(classificationText_u8());
    const auto iso8859_1 = str::testing::to_string(classificationText_w1252());
    const auto utf_8_u8 = classificationText_u8();
    const auto iso8859_1_u8 = str::to_u8string(classificationText_w1252());
    const auto utf_8_view = str::str<std::string>(classificationText_u8());
    const auto iso8859_1_view =  str::str<std::string>(str::to_u8string(classificationText_w1252()));

    {
        CHECK(iso8859_1 == utf_8);
        static const auto s = classificationText_platform();
        CHECK(iso8859_1 == s);
        CHECK(utf_8 == s);

        CHECK(iso8859_1_u8 == utf_8_u8);

        const auto expected = str::c_str<std::string>(classificationText_u8());
        CHECK(utf_8_view == expected);
        CHECK(iso8859_1_view == expected);
    }
    {
        CHECK(utf_8 == iso8859_1);
        static const auto s = classificationText_platform();
        CHECK(iso8859_1 == s);
        CHECK(utf_8 == s);

        CHECK(utf_8_u8 == iso8859_1_u8);

        const auto expected = str::c_str<std::string>(classificationText_u8());
        CHECK(utf_8_view == expected);
        CHECK(iso8859_1_view == expected);
    }
}
