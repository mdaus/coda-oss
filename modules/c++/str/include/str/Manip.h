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

#ifndef __STR_MANIP_H__
#define __STR_MANIP_H__

#include <ctype.h>
#include <wchar.h>

#include <string>
#include <vector>

#include "config/compiler_extensions.h"
#include "coda_oss/CPlusPlus.h"
#include "str/Convert.h"

namespace str
{
// non-const overload for .data() in C++17
template<typename CharT>
inline CharT* data(std::basic_string<CharT>& s) noexcept
{
    #if CODA_OSS_cpp17
    return s.data();
    #else
    CODA_OSS_disable_warning_push
    #if _MSC_VER
    #pragma warning(disable : 26492)  // Don't use const_cast to cast away const or volatile (type.3).
    #endif  
    return const_cast <std::basic_string<CharT>::pointer>(s.data());
    CODA_OSS_disable_warning_pop
    #endif // CODA_OSS_cpp17
}
template <typename CharT>
inline const CharT* data(const std::basic_string<CharT>& s) noexcept // to make generic programming easier
{
    return s.data();
}

/**
 *  Trim the white space off the back and front of a string
 *  @param  s  String to trim
 */
void trim(std::string& s);
extern std::string strip(const std::string& s);
extern std::string& strip(std::string& s);

/**
 *  Checks the end of s with match
 *  @param  s  String to check
 *  @param  match  String to compare with
 *  @return true if it matches, otherwise false
 */
bool endsWith(const std::string& s, const std::string& match);
extern bool ends_with(const std::string& s, const std::string&) noexcept;

/**
 *  Checks the start of s with match
 *  @param  s  String to check
 *  @param  s  String to compare with
 *  @return true if it matches, otherwise false
 */
bool startsWith(const std::string& s, const std::string& match);
extern bool starts_with(const std::string& s, const std::string&) noexcept;


/**
 *  finds the first instance of "search" and
 *  substitutes it for "replace", it then returns
 *  the start of search, so you can loop and replace
 *  all instances of "search". 
 *  @param  str     String to check
 *  @param  search  String to search for
 *  @param  replace String to replace with
 *  @param  start   starting position to start search
 *  @return position of first find, str.length() if not found
 */
size_t replace(std::string& str, 
               const std::string& search,
               const std::string& replace,
               size_t start = 0);

/**
 *  finds all instances of "search" and
 *  substitutes them for "replace"
 *  @param  string  String to check
 *  @param  search  String to search for
 *  @param  replace String to replace with
 */
void replaceAll(std::string& string, 
                const std::string& search,
                const std::string& replace);

/**
 * Returns true if the string contains the match
 */
bool contains(const std::string& str, const std::string& match);


/**
 * Returns true if the string contains only letters.
 */
bool isAlpha(const std::string& s);

/**
 * Returns true if the string contains only letters and spaces.
 */
bool isAlphaSpace(const std::string& s);

/**
 * Returns true if the string contains only digits. This does not include
 * decimal points.
 */
bool isNumeric(const std::string& s);

/**
 * Returns true if the string contains only digits and spaces.
 */
bool isNumericSpace(const std::string& s);

/**
 * Returns true if the string contains only whitespace characters (or empty).
 */
bool isWhitespace(const std::string& s);

/**
 * Returns true if the string contains only letters and digits.
 */
bool isAlphanumeric(const std::string& s);

/**
 * Returns true if the string contains only ASCII printable characters.
 */
bool isAsciiPrintable(const std::string& s);

/**
 * Returns true if the string contains only the given allowed characters.
 */
bool containsOnly(const std::string& s, const std::string& validChars);

/**
 *  Splits a string based on a splitter string. Similar to tokenization, except
 *  the splitter string can be of any length.
 *  @param  s         String to check
 *  @param  splitter  String to split upon
 *  @return vector of strings
 */
std::vector<std::string> split(const std::string& s,
                               const std::string& splitter = " ",
                               size_t maxSplit = std::string::npos);

//! Uses std::transform to convert all chars to lower case
//! Uses std::transform to convert all chars to upper case
void lower(std::string& s);
void upper(std::string& s);

/*!
 * Replaces any characters that are invalid in XML (&, <, >, ', ") with their
 * escaped counterparts
 */
void escapeForXML(std::string& str);

template<typename T>
std::string join(const std::vector<T>& toks, std::string with)
{
    if (toks.empty())
        return "";

    const auto len = static_cast<int>(toks.size());
    std::ostringstream oss;
    int i = 0;
    for (; i < len - 1; i++)
    {
        oss << str::toString<T>(toks[i]) << with;
    }
    oss << str::toString(toks[i]);
    return oss.str();
}


}

#endif
