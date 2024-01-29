/* =========================================================================
 * This file is part of str-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * (C) Copyright 2020, 2021, 2022, Maxar Technologies, Inc.
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
#include "str/Encoding.h"

#include <assert.h>
#include <string.h>
#if _WIN32
#include <comdef.h>  // _bstr_t
#endif

#include <map>
#include <locale>
#include <stdexcept>
#include <vector>
#include <iterator>
#include <string>
#include <set>

#include "gsl/gsl.h"
#include "config/compiler_extensions.h"

#include "str/Manip.h"
#include "str/Convert.h"
CODA_OSS_disable_warning_push
#if _MSC_VER
#pragma warning(disable: 26818) // Switch statement does not cover all cases. Consider adding a '...' label (es.79).
#else
CODA_OSS_disable_warning(-Wshadow)
#endif
#include "str/utf8.h"
CODA_OSS_disable_warning_pop

//// "sys" depends on "str" so can't use sys::PlatformType
//enum class PlatformType
//{
//    Windows,
//    Linux,
//    // MacOS
//};
#if _WIN32
//static constexpr auto Platform = PlatformType::Windows;
#elif defined(_POSIX_C_SOURCE)
//static constexpr auto Platform = PlatformType::Linux;
#else
#error "Unknown platform"
#endif

// Need to look up characters from \x80 (EURO SIGN) to \x9F (LATIN CAPITAL LETTER Y WITH DIAERESIS)
// in a map: http://www.unicode.org/Public/MAPPINGS/VENDORS/MICSFT/WINDOWS/CP1252.TXT
inline coda_oss::u8string utf8_(char32_t i)
{
    const auto ch = gsl::narrow<std::u32string::value_type>(i);
    return str::to_u8string(std::u32string{ch});
}

static const auto& Windows1252_x80_x9F_to_u8string_()
{
    static const std::map<char32_t, coda_oss::u8string> retval{
            {U'\x80', utf8_(U'\x20AC')}  // EURO SIGN
        // , {U'\x81, replacement_character } // UNDEFINED
        , {U'\x82', utf8_(U'\x201A') } // SINGLE LOW-9 QUOTATION MARK
        , {U'\x83', utf8_(U'\x0192') } // LATIN SMALL LETTER F WITH HOOK
        , {U'\x84', utf8_(U'\x201E') } // DOUBLE LOW-9 QUOTATION MARK
        , {U'\x85', utf8_(U'\x2026') } // HORIZONTAL ELLIPSIS
        , {U'\x86', utf8_(U'\x2020') } // DAGGER
        , {U'\x87', utf8_(U'\x2021') } // DOUBLE DAGGER
        , {U'\x88', utf8_(U'\x02C6') } // MODIFIER LETTER CIRCUMFLEX ACCENT
        , {U'\x89', utf8_(U'\x2030') } // PER MILLE SIGN
        , {U'\x8A', utf8_(U'\x0160') } // LATIN CAPITAL LETTER S WITH CARON
        , {U'\x8B', utf8_(U'\x2039') } // SINGLE LEFT-POINTING ANGLE QUOTATION MARK
        , {U'\x8C', utf8_(U'\x0152') } // LATIN CAPITAL LIGATURE OE
        //, {U'\x8D, replacement_character } // UNDEFINED
        , {U'\x8E', utf8_(U'\x017D') } // LATIN CAPITAL LETTER Z WITH CARON
        //, {U'\x8F, replacement_character } // UNDEFINED
        //, {U'\x90, replacement_character } // UNDEFINED
        , {U'\x91', utf8_(U'\x2018') } // LEFT SINGLE QUOTATION MARK
        , {U'\x92', utf8_(U'\x2019') } // RIGHT SINGLE QUOTATION MARK
        , {U'\x93', utf8_(U'\x201C') } // LEFT DOUBLE QUOTATION MARK
        , {U'\x94', utf8_(U'\x201D') } // RIGHT DOUBLE QUOTATION MARK
        , {U'\x95', utf8_(U'\x2022') } // BULLET
        , {U'\x96', utf8_(U'\x2013') } // EN DASH
        , {U'\x97', utf8_(U'\x2014') } // EM DASH
        , {U'\x98', utf8_(U'\x02DC') } // SMALL TILDE
        , {U'\x99', utf8_(U'\x2122') } // TRADE MARK SIGN
        , {U'\x9A', utf8_(U'\x0161') } // LATIN SMALL LETTER S WITH CARON
        , {U'\x9B', utf8_(U'\x203A') } // SINGLE RIGHT-POINTING ANGLE QUOTATION MARK
        , {U'\x9C', utf8_(U'\x0153') } // LATIN SMALL LIGATURE OE
        //, {U'\x9D, replacement_character } // UNDEFINED
        , {U'\x9E', utf8_(U'\x017E') } // LATIN SMALL LETTER Z WITH CARON
        , {U'\x9F', utf8_(U'\x0178') } // LATIN CAPITAL LETTER Y WITH DIAERESIS
    };
    return retval;
}
static auto Windows1252_to_u8string()
{
    auto retval = Windows1252_x80_x9F_to_u8string_();

    // Add the ASCII values to the map too.  1) We're already looking
    // in the map anyway for Windows-1252 characters. 2) Need map
    // entires for conversion from UTF-8 to Windows-1252.
    for (char32_t ch = U'\x00'; ch < U'\x80'; ch++)
    {
        coda_oss::u8string s {static_cast<coda_oss::u8string::value_type>(ch)};
        retval[ch] = std::move(s);    
    }

    // Ditto for ISO8859-1 ...
    for (char32_t ch = U'\xA0'; ch <= U'\xff'; ch++)
    {
        // ISO8859-1 can be converted to UTF-8 with bit-twiddling
      
        // https://stackoverflow.com/questions/4059775/convert-iso-8859-1-strings-to-utf-8-in-c-c
        // *out++=0xc2+(*in>0xbf), *out++=(*in++&0x3f)+0x80;
        const auto b1 = 0xc2 + (ch > 0xbf);
        const auto b2 = (ch & 0x3f) + 0x80;
        coda_oss::u8string s {static_cast<coda_oss::u8string::value_type>(b1)};
        s += coda_oss::u8string {static_cast<coda_oss::u8string::value_type>(b2)};
        retval[ch] = std::move(s);    
    }

    return retval;
}

inline void append(std::string& result, const coda_oss::u8string& utf8)
{
    result += str::str<std::string>(utf8);
}
inline void append(coda_oss::u8string& result, const coda_oss::u8string& utf8)
{
    result += utf8;
}
template<typename CharT>
inline void append(std::basic_string<CharT>& result, const coda_oss::u8string& utf8)
{
    auto p = str::c_str<std::string>(utf8);
    utf8::utf8to16(p, p + utf8.size(), std::back_inserter(result));
}
inline void append(std::u32string& result, const coda_oss::u8string& utf8)
{
    auto p = str::c_str<std::string>(utf8);
    utf8::utf8to32(p, p + utf8.size(), std::back_inserter(result));
}

static const auto& get_Windows1252_undefined()
{
    // https://en.wikipedia.org/wiki/Windows-1252
    // > According to the information on Microsoft's and the Unicode Consortium's
    // > websites, positions 81, 8D, 8F, 90, and 9D are unused; ...
    static const std::set<uint8_t> retval{0x81, 0x8d, 0x8f, 0x90, 0x9d};
    return retval;
}
template<typename TChar>
static void fromWindows1252_(str::W1252string::value_type ch, std::basic_string<TChar>& result, bool strict=false)
{
    static const auto map = Windows1252_to_u8string();
    const auto ch32 = static_cast<std::u32string::value_type>(ch);
    const auto it = map.find(ch32);
    if (it != map.end())
    {
        append(result, it->second);
        return;
    }

    static const auto& w1252_undefined = get_Windows1252_undefined();
    const auto i8 = static_cast<uint8_t>(ch);
    if (w1252_undefined.find(i8) == w1252_undefined.end())
    {
        throw std::logic_error("Unhandled Windows-1252 character."); // this shouldn't happen
    }
    if (strict)
    {
        // If the input text contains a character that isn't defined in Windows-1252; return a
        // "replacement character."  Yes, this will  **corrupt** the input data as information is lost:
        // https://en.wikipedia.org/wiki/Specials_(Unicode_block)#Replacement_character
        //
        // Or ... https://en.wikipedia.org/wiki/Windows-1252
        // > ... however, the Windows API `MultiByteToWideChar` maps these to the corresponding
        // > C1 control codes. The "best fit" mapping documents this behavior, too.
        static const auto replacement_character = utf8_(U'\xfffd');
        append(result, replacement_character);
    }
    else
    {
        append(result, utf8_(ch32)); // _bstr_t just preserves these values, do the same
    }
}
template <typename TChar>
static auto make_Windows1252_lookup(bool strict)
{
    std::vector<std::basic_string<TChar>> retval(0xff+1);
    for (size_t i = 0; i <= 0xff; i++)
    {
        const auto ch = static_cast<str::W1252string::value_type>(i);
        fromWindows1252_(ch, retval[i], strict);
    }
    return retval;
}
template <typename TChar>
static const auto& getLookup(bool strict)
{
    if (strict)
    {
        static const auto lookup = make_Windows1252_lookup<TChar>(strict);
        return lookup; 
    }
    else
    {
        static const auto lookup = make_Windows1252_lookup<TChar>(strict);
        return lookup; 
    }
}

template<typename TChar>
static void w1252_to_string_(str::W1252string::const_pointer p, size_t sz, std::basic_string<TChar>& result, bool strict = false)
{
    auto&& lookup = getLookup<TChar>(strict);
    for (size_t i = 0; i < sz; i++)
    {
        const auto ch = static_cast<ptrdiff_t>(p[i]);
        result += lookup[ch];
    }
}
inline void w1252_to_string(str::W1252string::const_pointer p, size_t sz, std::u16string& result)
{
    w1252_to_string_(p, sz, result);
}
inline void w1252_to_string(str::W1252string::const_pointer p, size_t sz, std::u32string& result)
{
    w1252_to_string_(p, sz, result);
}
inline void w1252to8(str::W1252string::const_pointer p, size_t sz, std::string& result)
{
    w1252_to_string_(p, sz, result);
}
inline void w1252to8(str::W1252string::const_pointer p, size_t sz, coda_oss::u8string& result)
{
    w1252_to_string_(p, sz, result);
}

inline void w1252to16(str::W1252string::const_pointer p, size_t sz, std::u16string& result)
{
    w1252_to_string(p, sz, result);

    #if defined(_WIN32) && (!defined(_NDEBUG) || defined(DEBUG))
    const _bstr_t bstr(std::string(str::details::cast<const char*>(p), sz).c_str()); // no _bstr_t ctor taking sz
    const std::wstring wstr(static_cast<const wchar_t*>(bstr));
    assert(result == str::str<std::u16string>(wstr));
    #endif
}
inline void w1252to32(str::W1252string::const_pointer p, size_t sz, std::u32string& result)
{
    w1252_to_string(p, sz, result);
}

template<typename TChar>
static auto make_u16_map(bool strict)
{
    // Find the corresponding UTF-16 value for every Windows-1252 input;
    // obviously, most UTF-16 values can't be converted.
    const auto& lookup = getLookup<std::u16string::value_type>(strict);

    std::map<std::u16string::value_type, TChar> retval;
    for (size_t i = 0; i <= 0xff; i++)  // **not** `uint8_t` to avoid wrap-around
    {
        const auto u16 = lookup[i];
        assert(u16.length() == 1); // all values in Basic Multi-lingual Plane (BMP); no emojis, etc.
        const auto ch = static_cast<TChar>(i);
        retval[u16[0]] = ch;
    }
    return retval;
}
template <typename TChar>
static const auto& get_u16_map(bool strict)
{
    if (strict)
    {
        static const auto lookup = make_u16_map<TChar>(strict);
        return lookup;
    }
    else
    {
        static const auto lookup = make_u16_map<TChar>(strict);
        return lookup;
    }
}

template <typename TChar>
static auto make_utf8_lookup(bool strict)
{
    // Find the corresponding UTF-8 value for every Windows-1252 input.
    auto&& lookup = get_u16_map<TChar>(strict);

    // Convert UTF-16 to UTF-8
    std::map<coda_oss::u8string, TChar> retval;
    for (auto&& kv : lookup)
    {
        retval[utf8_(kv.first)] = kv.second;
    }
    return retval;
}
template <typename TChar>
static auto&& get_utf8_lookup(bool strict)
{
    if (strict)
    {
        static const auto lookup = make_utf8_lookup<TChar>(strict);
        return lookup;
    }
    else
    {
        static const auto lookup = make_utf8_lookup<TChar>(strict);
        return lookup;
    }
}

template<typename TMap, typename TUtf, typename TChar>
static inline bool utf_to_1252_(const TMap& map, const TUtf& utf, std::basic_string<TChar>& result, bool strict)
{
    using value_type = TChar;

    const auto it = map.find(utf);
    if (it != map.end())
    {
        const auto w1252 = static_cast<value_type>(it->second);

        static const auto& w1252_undefined = get_Windows1252_undefined();
        const auto i8 = static_cast<uint8_t>(w1252);
        const auto is_undefined = w1252_undefined.find(i8) == w1252_undefined.end();

        // only care about undefined for `strict`
        if (!is_undefined || !strict)
        {
            result += w1252;
            return true; // not undefined or not strict
        }
    }

    // Either not in map, or the Windows1252 character is undefined.
    if (strict)
    {
        throw std::invalid_argument("UTF sequence can't be converted to Windows-1252.");
    }

    return false;
}
static inline void utf16to1252_(std::u16string::value_type utf, std::string& result, bool strict)
{
    using value_type = std::string::value_type;
    auto&& map = get_u16_map<value_type>(strict);
    if (utf_to_1252_(map, utf, result, strict))
    {
        return; // successful conversion
    }

    assert("UTF-16 sequence can't be converted to Windows-1252." && 0);
    result += static_cast<value_type>(0x7F);  // <DEL>
}
template<typename TChar>
static void utf8to1252_(const coda_oss::u8string& utf, std::basic_string<TChar>& result, bool strict)
{
    using value_type = TChar;

    auto&& map = get_utf8_lookup<TChar>(strict);
    if (utf_to_1252_(map, utf, result, strict))
    {
        return;  // successful conversion
    }

    // Either 1) not in map, or 2) 'undefiend' with strict=true
    if (utf.length() == 2)  // _bstr_t preserves these values
    {
        result += static_cast<value_type>(utf[0]);
        result += static_cast<value_type>(utf[1]);
    }
    else
    {
        assert("UTF-8 sequence can't be converted to Windows-1252." && 0);
        result += static_cast<value_type>(0x7F);  // <DEL>
    }
}

static void get_next_utf8_byte(coda_oss::u8string::const_pointer p, size_t sz,
    size_t& i,  coda_oss::u8string& utf8)
{
    if (!(i + 1 < sz))
    {
        throw std::invalid_argument("No remaining bytes, invalid UTF-8 encoding.");
    }
    i++;  // move to next byte

    // Bytes 2, 3 and 4 are always >= 0x80 (10xxxxxx), see https://en.wikipedia.org/wiki/UTF-8
    const auto b = static_cast<uint8_t>(p[i]);
    if (b < static_cast<uint8_t>(0x80))  // 10xxxxxx
    {
        throw std::invalid_argument("Invalid next byte in UTF-8 encoding.");
    }
    utf8 += coda_oss::u8string{static_cast<coda_oss::u8string::value_type>(b)};
}
static void get_utf8_string(coda_oss::u8string::const_pointer p, size_t sz, size_t& i, coda_oss::u8string& utf8)
{
    const auto b1 = static_cast<uint8_t>(p[i]);
    if (b1 >= 0x80)  // 0xxxxxxx
    {
        get_next_utf8_byte(p, sz, i, utf8);
        if (b1 >= 0xE0)  // 1110xxxx
        {
            // should be a 3- or 4-byte sequence
            get_next_utf8_byte(p, sz, i, utf8);
            if (b1 >= 0xF0)  // 1111xxx
            {
                // should be a 4-byte sequence
                get_next_utf8_byte(p, sz, i, utf8);
            }
        }
    }
}
template<typename TChar>
static void utf8to1252(coda_oss::u8string::const_pointer p, size_t sz, std::basic_string<TChar>& result, bool strict=false)
{
    for (size_t i = 0; i < sz; i++)
    {
        const auto b1 = static_cast<uint8_t>(p[i]);
        auto utf8 = coda_oss::u8string{static_cast<coda_oss::u8string::value_type>(b1)};
        get_utf8_string(p, sz, i, utf8);

        utf8to1252_(utf8, result, strict);
    }
}

static inline void utf16to1252(std::u16string::const_pointer p, size_t sz, std::string& result, bool strict=false)
{
    for (size_t i = 0; i < sz; i++)
    {
        const auto ch = p[i];
        utf16to1252_(ch, result, strict);
    }
}

struct back_inserter final
{ 
    coda_oss::u8string* container = nullptr; // pointer instead of reference for copy
    explicit back_inserter(coda_oss::u8string& s) noexcept : container(&s) { }

    back_inserter& operator=(uint8_t v)
    {
        container->push_back(static_cast<coda_oss::u8string::value_type>(v));
        return *this;
    }
    back_inserter& operator*() noexcept { return *this; }
    back_inserter operator++(int) noexcept { return *this; }
};

template <typename TBasicStringT, typename CharT>
inline auto to_uXXstring(const std::basic_string<CharT>& s)
{
    const auto p = str::c_str<TBasicStringT>(s);
    #if _WIN32
    return str::to_u16string(p, s.length()); // std::wstring is UTF-16 on Windows
    #else
    return str::to_u32string(p, s.length()); // assume std::wstring is UTF-32 everywhere except Windows
    #endif
}
template<typename CharT>
static std::wstring to_wstring_(const std::basic_string<CharT>& s, bool is_utf8)
{
   
    const auto result = is_utf8 ? to_uXXstring<coda_oss::u8string>(s)
                                : to_uXXstring<str::W1252string>(s);
    return str::str<std::wstring>(result);
}

/***********************************************************************************/

std::string str::details::to_string(const coda_oss::u8string& s)
{
    #if _WIN32
    std::string retval;
    utf8to1252(s.c_str(), s.length(), retval);
    return retval;
    #else
    return str<std::string>(s);
    #endif
}

std::string str::testing::to_string(const str::W1252string& s)
{
    #if _WIN32
    return str<std::string>(s);
    #else
    std::string retval;
    w1252to8(s.c_str(), s.length(), retval);
    return retval;
    #endif
}

std::string str::details::to_string(const std::wstring& s)
{
    const auto p = details::c_str(s);
    const auto sz = s.length();

    std::string retval;
    #if _WIN32
    utf16to1252(p, sz, retval); // UTF16 -> Windows-1252 on Windows.
    #else
    utf8::utf32to8(p, p + sz, std::back_inserter(retval)); // UTF32 -> UTF-8 everywhere else.
    #endif   
    return retval;
}

std::wstring str::details::to_wstring(const std::string& s)
{
     #if _WIN32
    return to_wstring_(s, false /*is_utf8*/); // Input is Windows-1252 on Windows
    #else
    return to_wstring_(s, true /*is_utf8*/);  // Input is UTF-8 everywhere except Windows
    #endif
}
std::wstring str::details::to_wstring(const coda_oss::u8string& s)
{
    return to_wstring_(s, true /*is_utf8*/);
}
std::wstring str::testing::to_wstring(const str::W1252string& s)
{
    return to_wstring_(s, false /*is_utf8*/);
}

/***********************************************************************************/

std::u16string str::to_u16string(str::W1252string::const_pointer p, size_t sz)
{
    std::u16string retval;
    w1252to16(p, sz, retval);
    return retval;
}
std::u32string str::to_u32string(str::W1252string::const_pointer p, size_t sz)
{
    std::u32string retval;
    w1252to32(p, sz, retval);
    return retval;
}

str::W1252string str::to_w1252string(coda_oss::u8string::const_pointer p, size_t sz)
{
    str::W1252string retval;
    utf8to1252(p, sz, retval);
    return retval;
}

coda_oss::u8string str::to_u8string(std::u16string::const_pointer p, size_t sz)
{
    coda_oss::u8string retval;
    utf8::utf16to8(p, p + sz, back_inserter(retval));
    return retval;
}

std::u16string str::to_u16string(coda_oss::u8string::const_pointer p_, size_t sz)
{
    auto p = details::cast<std::string::const_pointer>(p_);
    std::u16string retval;
    utf8::utf8to16(p, p + sz, std::back_inserter(retval));
    return retval;
}

std::u32string str::to_u32string(coda_oss::u8string::const_pointer p_, size_t sz)
{
    auto p = details::cast<std::string::const_pointer>(p_);
    std::u32string retval;
    utf8::utf8to32(p, p + sz, std::back_inserter(retval));
    return retval;
}

coda_oss::u8string str::to_u8string(std::u32string::const_pointer p, size_t sz)
{
    coda_oss::u8string retval;
    utf8::utf32to8(p, p + sz, back_inserter(retval));
    return retval;
}

coda_oss::u8string str::to_u8string(W1252string::const_pointer p, size_t sz)
{
    coda_oss::u8string retval;
    w1252to8(p, sz, retval);
    return retval;
}
